/**
 *
 * Copyright (C) 2021-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <iomanip>

#include <git2.h>

#include <talipot/MaterialDesignIcons.h>
#include <talipot/PluginHeaders.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

// enable to use git_oid as unordered_map key
namespace std {
template <>
struct hash<const git_oid> {
  size_t operator()(const git_oid &oid) const {
    string sha1_git = git_oid_tostr_s(&oid);
    return stoul(sha1_git.substr(0, 7), nullptr, 16);
  }
};
}

static bool operator==(const git_oid &oid, const git_oid &oid2) {
  return git_oid_equal(&oid, &oid2);
}

// format git_time to ISO 8601 string
static string iso8601(const git_time *gt) {
  ostringstream oss;
  oss << put_time(gmtime(reinterpret_cast<const time_t *>(&gt->time)), "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

static constexpr string_view paramHelp[] = {
    // directory
    "Path to a git repository",

    // root commit
    "The root commit to start walking on the commits graph, if empty HEAD will be used.",

    // stop commit
    "The commit to stop walking on the commits graph, if empty all commits will be walked.",

    // tree subgraphs
    "If true, extract the targeted tree of each imported commit and add it as a subgraph"};

static const string defaultStopCommit = "HEAD~1000";

/**
 *
 * Talipot plugin to import the graph of git objects (commits, trees, blobs)
 * from a local repository.
 *
 * Branches and tags will also be imported as subgraphs of the commits graph.
 *
 * Tree targeted by each commit can also be optionally extracted and added
 * as a subgraph.
 *
 */
class GitImport : public ImportModule {
public:
  PLUGININFORMATION("Git", "Antoine Lambert", "20/05/2021",
                    "Imports git objects graph from a repository.", "1.0", "Software")
  GitImport(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<string>("dir::directory", paramHelp[0].data(), "");
    addInParameter<string>("root commit", paramHelp[1].data(), "HEAD");
    addInParameter<string>("stop commit", paramHelp[2].data(), defaultStopCommit);
    addInParameter<bool>("tree subgraphs", paramHelp[3].data(), "false");
  }
  ~GitImport() override = default;

  string icon() const override {
    return TalipotBitmapDir + "git.png";
  }

  git_oid addCommitNode(git_oid commitOid) {
    // get commit info
    git_commit *commit = nullptr;
    git_commit_lookup(&commit, repo, &commitOid);
    const git_signature *author = git_commit_author(commit);
    const git_signature *committer = git_commit_committer(commit);
    git_oid treeOid = *git_commit_tree_id(commit);

    // create commit node
    node n = graph->addNode();
    oidNode[commitOid] = n;
    string sha1_git = git_oid_tostr_s(&commitOid);
    // save commit data in dedicated properties
    (*graph)["viewLabel"][n] = sha1_git.substr(0, 7);
    (*graph)["object_type"][n] = "commit";
    (*graph)["sha1_git"][n] = sha1_git;
    (*graph)["author_name"][n] = author->name;
    (*graph)["author_email"][n] = author->email;
    (*graph)["author_date"][n] = iso8601(&author->when);
    (*graph)["committer_name"][n] = committer->name;
    (*graph)["committer_email"][n] = committer->email;
    (*graph)["committer_date"][n] = iso8601(&committer->when);
    (*graph)["tree"][n] = git_oid_tostr_s(&treeOid);
    string message = git_commit_message(commit);
    (*graph)["message"][n] = message.substr(0, message.size() - 1);
    // set commit node visual attributes
    (*graph)["viewIcon"][n] = MaterialDesignIcons::SourceCommit;
    (*graph)["viewRotation"][n] = 90.0;
    (*graph)["viewColor"][n] = Color::Blue;
    // process commit parents
    vector<string> parents;
    unsigned int nbParents = git_commit_parentcount(commit);
    for (unsigned int i = 0; i < nbParents; ++i) {
      const git_oid parentOid = *git_commit_parent_id(commit, i);
      if (oidNode.find(parentOid) != oidNode.end()) {
        // create node for parent commit
        commitsSg->addNodes({oidNode[parentOid], n});
        // connect commit node to parent node
        commitsSg->addEdge(n, oidNode[parentOid]);
      }
      parents.push_back(git_oid_tostr_s(&parentOid));
    }
    (*graph)["parents"][n] = parents;

    git_commit_free(commit);
    return treeOid;
  }

  void addTreeAndBlobNodes(const git_oid rootTreeOid) {
    if (const auto it = oidNode.find(rootTreeOid); it != oidNode.end()) {
      // tree already processed, nothing to do
      return;
    }

    // create root tree node
    node n = addTreeNode(rootTreeOid);
    if (treeSubgraphs) {
      // get tree sha1_git
      string currentRootTreeId = git_oid_tostr_s(&rootTreeOid);
      // create subgraph for tree
      currentRootTreeSg = rootTreesSg->addSubGraph(currentRootTreeId);
      currentRootTreeSg->addNode(n);
    }
    treesBlobsSg->addNode(n);
    (*graph)["viewLabel"][n] = "/";
    treePathToNode.clear();
    treePathToNode[""] = n;
    // walk on tree content recursively
    git_tree *tree = nullptr;
    git_tree_lookup(&tree, repo, &rootTreeOid);
    git_tree_walk(tree, GIT_TREEWALK_PRE, &GitImport::gitTreewalkCb, this);
    git_tree_free(tree);
  }

  // tree walker callback called for each reachable node
  static int gitTreewalkCb(const char *root, const git_tree_entry *entry, void *payload) {
    int ret = 0;
    GitImport *gitImport = static_cast<GitImport *>(payload);
    git_oid entryOid = *git_tree_entry_id(entry);
    node rootNode = gitImport->treePathToNode[root];
    string entryName = git_tree_entry_name(entry);
    string entryPath = root + entryName + "/";
    node n;
    if (git_tree_entry_type(entry) == GIT_OBJ_TREE) {
      if (const auto it = gitImport->oidNode.find(entryOid); it != gitImport->oidNode.end()) {
        // we already walked on that subtree, no need to walk deeper on that path
        ret = 1;
      }
      // add or get tree node
      n = gitImport->addTreeNode(entryOid);
    } else if (git_tree_entry_type(entry) == GIT_OBJ_BLOB) {
      // add or get blob node
      n = gitImport->addBlobNode(entryOid);
    }
    if (n.isValid()) {
      edge e = gitImport->graph->addEdge(rootNode, n);
      gitImport->treesBlobsSg->addNode(n);
      gitImport->treesBlobsSg->addEdge(e);
      (*gitImport->graph)["name"][n] = entryName;
      (*gitImport->graph)["path"][n] = entryPath;
      (*gitImport->graph)["viewLabel"][n] = entryName;
      gitImport->treePathToNode[entryPath] = n;

      // add node and edge to root tree subgraph
      Graph *treeSg = gitImport->currentRootTreeSg;
      if (treeSg) {
        treeSg->addNode(n);
        treeSg->addEdge(e);
        if (ret > 0) {
          // if we already walk on that subtree, add reachable nodes and edges
          // in root tree subgraph
          for (auto e : gitImport->graph->dfsEdges(n, true)) {
            treeSg->addNode(gitImport->graph->target(e));
            treeSg->addEdge(e);
          }
        }
      }
    }
    return ret;
  }

  node addTreeNode(const git_oid treeOid) {
    // tree already processed, return its node
    if (const auto it = oidNode.find(treeOid); it != oidNode.end()) {
      return it->second;
    }

    // fetch tree data
    git_tree *tree = nullptr;
    git_tree_lookup(&tree, repo, &treeOid);
    unsigned int nbTreeEntries = git_tree_entrycount(tree);

    // reserve new graph elements
    graph->reserveNodes(graph->numberOfNodes() + nbTreeEntries);
    graph->reserveEdges(graph->numberOfEdges() + nbTreeEntries);
    treesBlobsSg->reserveNodes(treesBlobsSg->numberOfNodes() + nbTreeEntries);
    treesBlobsSg->reserveEdges(treesBlobsSg->numberOfEdges() + nbTreeEntries);
    if (currentRootTreeSg) {
      currentRootTreeSg->reserveNodes(currentRootTreeSg->numberOfNodes() + nbTreeEntries);
      currentRootTreeSg->reserveEdges(currentRootTreeSg->numberOfEdges() + nbTreeEntries);
    }

    // create tree node
    node n = graph->addNode();
    oidNode[treeOid] = n;

    // set some node properties
    (*graph)["object_type"][n] = "tree";
    (*graph)["sha1_git"][n] = git_oid_tostr_s(&treeOid);
    (*graph)["viewIcon"][n] = MaterialDesignIcons::Folder;
    (*graph)["viewColor"][n] = Color::JungleGreen;

    git_tree_free(tree);
    return n;
  }

  node addBlobNode(const git_oid blobOid) {
    // blob node already processed, return its node
    if (const auto it = oidNode.find(blobOid); it != oidNode.end()) {
      return it->second;
    }

    // fetch blob data
    git_blob *blob = nullptr;
    git_blob_lookup(&blob, repo, &blobOid);

    // create blob node
    node n = graph->addNode();
    oidNode[blobOid] = n;

    // set some node properties
    (*graph)["object_type"][n] = "blob";
    (*graph)["sha1_git"][n] = git_oid_tostr_s(&blobOid);
    (*graph)["viewIcon"][n] = MaterialDesignIcons::FileDocument;
    (*graph)["viewColor"][n] = Color::Red;

    git_blob_free(blob);
    return n;
  }

  bool gitError() const {
    if (pluginProgress) {
      pluginProgress->setError(giterr_last()->message);
    }
    git_libgit2_shutdown();
    return false;
  }

  bool importGraph() override {

    if (!dataSet) {
      return false;
    }

    // get git repository path
    string gitRepoPath;
    dataSet->get("dir::directory", gitRepoPath);

    // set graph name
    vector<string> pathParts = tokenize(gitRepoPath, "/");
    graph->setName(pathParts.back() + ".git");

    // create commits and tree + blob subgraphs
    commitsSg = graph->addSubGraph("commits");
    treesBlobsSg = graph->addSubGraph("trees and blobs");

    (*graph)["viewShape"].setAllNodeValue(NodeShape::Icon);

    // get start and stop commits
    string rootCommit = "HEAD";
    string stopCommit;
    dataSet->get("root commit", rootCommit);
    dataSet->get("stop commit", stopCommit);

    dataSet->get("tree subgraphs", treeSubgraphs);
    if (treeSubgraphs) {
      rootTreesSg = graph->addSubGraph("root trees");
    }

    git_libgit2_init();

    if (git_repository_open(&repo, gitRepoPath.c_str()) < 0) {
      return gitError();
    }

    // parse root commit expression
    git_object *rootCommitObj;
    if (git_revparse_single(&rootCommitObj, repo, rootCommit.c_str()) < 0) {
      return gitError();
    }

    // parse stop commit expression
    git_object *stopCommitObj = nullptr;
    if (!stopCommit.empty() && git_revparse_single(&stopCommitObj, repo, stopCommit.c_str()) < 0) {
      if (stopCommit == defaultStopCommit) {
        // repositories has less than 1000 commits, all history will be walked
        stopCommit = "";
      } else {
        return gitError();
      }
    } else if (stopCommitObj) {
      git_object_free(stopCommitObj);
    }

    // if stop commit is empty all history will be walked,
    // otherwise restrain the walk to the specified commits range
    string commitsRange;
    if (!stopCommit.empty()) {
      commitsRange = stopCommit + ".." + rootCommit;
    }

    // setup first walk to get commit counts
    git_revwalk *walker = nullptr;
    git_revwalk_new(&walker, repo);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);

    if (!commitsRange.empty() && git_revwalk_push_range(walker, commitsRange.c_str()) < 0) {
      return gitError();
    } else if (git_revwalk_push(walker, git_object_id(rootCommitObj)) < 0) {
      git_object_free(rootCommitObj);
      return gitError();
    }

    git_oid commitOid;

    unsigned int nbCommits = 0;
    while (!git_revwalk_next(&commitOid, walker)) {
      nbCommits++;
    }

    // reserve commits graph to import
    graph->reserveNodes(nbCommits);
    graph->reserveEdges(nbCommits);
    commitsSg->reserveNodes(nbCommits);
    commitsSg->reserveEdges(nbCommits);

    // setup second walk for graph import
    git_revwalk_reset(walker);
    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);

    if (!commitsRange.empty()) {
      git_revwalk_push_range(walker, commitsRange.c_str());
    } else {
      git_revwalk_push(walker, git_object_id(rootCommitObj));
      git_object_free(rootCommitObj);
    }

    // walk on each commit
    unsigned int i = 0;
    while (!git_revwalk_next(&commitOid, walker)) {
      // create commit node and link to parents
      git_oid rootTreeOid = addCommitNode(commitOid);
      // add commit target tree elements recursively
      addTreeAndBlobNodes(rootTreeOid);
      // connect commit to tree
      graph->addEdge(oidNode[commitOid], oidNode[rootTreeOid]);

      if (pluginProgress) {
        if (pluginProgress->progress(++i, nbCommits) != TLP_CONTINUE) {
          return false;
        }
        pluginProgress->setComment(to_string(i) + " / " + to_string(nbCommits) +
                                   " commits processed");
      }
    }

    // process branches and lightweight tags, create a subgraph
    // of the commits graph for each one of them
    git_reference_iterator *iter = NULL;
    int error = git_reference_iterator_new(&iter, repo);
    git_reference *ref = NULL;
    Graph *branchesSg = nullptr;
    Graph *tagsSg = nullptr;
    while (!(error = git_reference_next(&ref, iter))) {
      string refName = git_reference_name(ref);
      git_oid target;
      git_reference_name_to_id(&target, repo, refName.c_str());
      Graph *rootSg = nullptr;
      if (const auto it = oidNode.find(target); it != oidNode.end()) {

        if (pluginProgress) {
          pluginProgress->setComment("Processing reference named " + refName);
        }

        if (git_reference_is_branch(ref) || git_reference_is_remote(ref)) {
          if (!branchesSg) {
            branchesSg = commitsSg->addCloneSubGraph("branches");
          }
          rootSg = branchesSg;
        } else if (git_reference_is_tag(ref)) {
          if (!tagsSg) {
            tagsSg = commitsSg->addCloneSubGraph("tags");
          }
          rootSg = tagsSg;
        }
        if (rootSg) {
          vector<node> refCommits = iteratorVector(commitsSg->bfs(it->second, true));
          rootSg->inducedSubGraph(refCommits, nullptr, refName);
        }
      }
    }
    git_reference_iterator_free(iter);

    if (error != GIT_ITEROVER) {
      return gitError();
    }

    // process annotated tags, create a subgraph of the commits graph
    // for each one of them
    git_strarray tags;
    git_tag_list(&tags, repo);

    for (unsigned int i = 0; i < tags.count; ++i) {
      string tagName = tags.strings[i];
      string refName = "refs/tags/" + tagName;
      git_reference *ref;
      git_reference_lookup(&ref, repo, refName.c_str());
      git_tag *tag = nullptr;
      git_reference_peel(reinterpret_cast<git_object **>(&tag), ref, GIT_OBJ_TAG);
      if (!tag || git_tag_target_type(tag) != GIT_OBJ_COMMIT) {
        continue;
      }
      git_oid target = *git_tag_target_id(tag);
      if (const auto it = oidNode.find(target); it != oidNode.end()) {

        if (pluginProgress) {
          pluginProgress->setComment("Processing tag named " + tagName);
        }

        if (!tagsSg) {
          tagsSg = commitsSg->addCloneSubGraph("tags");
        }

        vector<node> refCommits = iteratorVector(commitsSg->bfs(it->second, true));
        tagsSg->inducedSubGraph(refCommits, nullptr, tagName);
      }
    }

    // cleanup
    git_revwalk_free(walker);
    git_repository_free(repo);

    git_libgit2_shutdown();

    // draw imported graphs
    string err;
    graph->applyPropertyAlgorithm("Fast Multipole Multilevel Embedder (OGDF)",
                                  graph->getLayoutProperty("viewLayout"), err);
    commitsSg->applyPropertyAlgorithm("Sugiyama (OGDF)",
                                      commitsSg->getLocalLayoutProperty("viewLayout"), err);

    return true;
  }

  git_repository *repo = nullptr;
  unordered_map<const git_oid, node> oidNode;
  unordered_map<string, node> treePathToNode;
  Graph *commitsSg = nullptr;
  Graph *treesBlobsSg = nullptr;
  Graph *rootTreesSg = nullptr;
  Graph *currentRootTreeSg = nullptr;
  bool treeSubgraphs = false;
};

PLUGIN(GitImport)
