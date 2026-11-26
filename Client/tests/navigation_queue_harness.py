"""Real navigation heaps and graph searches under invalid and reordered input."""
import re


def harness(game, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#(?:include|pragma)[^\n]*", "", text)

    graph = game / "Src/GraphUtil"
    parts = [r'''
#include <algorithm>
#include <set>
#include <random>
#include <fstream>
#include <stdexcept>
namespace NavigationTests {
enum { invalid_node_index = -1, INVALID_MAP_ID = -1 };
''', clean(source(graph / "PriorityQueue.h")), clean(source(graph / "SparseGraph.h")),
             clean(source(graph / "GraphAlgorithms.h"))]
    nodes = clean(source(game / "Src/MMONavigationManager.h"))
    parts.extend(function(nodes, "class " + name + "\n") + ";" for name in ["NaviNode", "NaviLink"])
    from sparse_graph_harness import fixture
    parts.append(fixture())
    parts.append(r'''
template<class T> void CheckBasic() {
    for (int capacity : {-2147483647 - 1, -1, 0, 1, 7, 31}) {
        T queue(capacity);
        assert(queue.empty() && queue.pop() == 0);
        assert((capacity > 0) == queue.insert(1));
        if (capacity > 0) assert(queue.pop() == 1);
        std::multiset<int> expected;
        std::mt19937 random(773);
        for (int step = 0; step < 2000; ++step) {
            if (random() % 3) {
                int value = int(random() % 200) - 100;
                bool inserted = queue.insert(value);
                bool room = capacity > 0 && expected.size() < (size_t)capacity;
                assert(inserted == room);
                if (inserted) expected.insert(value);
            } else {
                const int value = queue.pop();
                if (expected.empty()) assert(value == 0);
                else {
                    assert(expected.count(value));
                    expected.erase(expected.find(value));
                }
            }
            assert(queue.empty() == expected.empty());
        }
    }
}
struct Heuristic {
    template<class Graph> static double Calculate(const Graph& graph, int target, int node) {
        assert(target >= 0 && target < graph.NumNodes() && node >= 0 && node < graph.NumNodes());
        return 0.0;
    }
};
struct BadGraph {
    using EdgeType = NaviLink;
    std::vector<std::vector<NaviLink>> edges;
    BadGraph() : edges(3) {
        edges[0] = {NaviLink(0, -1), NaviLink(0, 2147483647), NaviLink(0, 1, 2)};
        edges[1] = {NaviLink(1, 2, 3)};
    }
    int NumNodes() const { return (int)edges.size(); }
    struct ConstEdgeIterator {
        const std::vector<NaviLink>& links;
        size_t index = 0;
        ConstEdgeIterator(const BadGraph& graph, int node) : links(graph.edges.at(node)) {}
        const NaviLink* begin() { index = 0; return end() ? nullptr : &links[index]; }
        const NaviLink* next() { ++index; return end() ? nullptr : &links[index]; }
        bool end() const { return index >= links.size(); }
    };
};
void Run() {
    CheckSparseGraph();
    CheckBasic<PriorityQ<int>>(); CheckBasic<PriorityQLow<int>>();
    PriorityQ<int> high(5); PriorityQLow<int> low(5);
    for (int value : {8, 1, 5, -3, 9}) { assert(high.insert(value)); assert(low.insert(value)); }
    assert(!high.insert(4) && !low.insert(4));
    for (int value : {9, 8, 5, 1, -3}) assert(high.Peek() == value && high.pop() == value);
    for (int value : {-3, 1, 5, 8, 9}) assert(low.peek() == value && low.pop() == value);
    assert(!high.pop() && !low.pop() && !high.Peek() && !low.peek());
    std::vector<int> empty;
    ReorderUpwards(empty, 2147483647); ReorderUpwards(empty, -1);
    ReorderDownwards(empty, 0, 2147483647); ReorderDownwards(empty, 1, 0);
    std::vector<int> keys(64);
    for (int i = 0; i < 64; ++i) keys[i] = i;
    for (int capacity : {-2147483647 - 1, -1, 0, 1, 8, 64, 2147483647}) {
        IndexedPriorityQLow<int> queue(keys, capacity);
        std::set<int> active;
        std::mt19937 random(991);
        for (int invalid : {-2147483647 - 1, -1, 64, 2147483647}) {
            assert(!queue.insert(invalid) && !queue.ChangePriority(invalid) && !queue.contains(invalid));
        }
        assert(queue.Pop() == -1);
        for (int step = 0; step < 4000; ++step) {
            int idx = int(random() % keys.size());
            int operation = int(random() % 3);
            if (operation == 0) {
                const bool room = capacity > 0 && active.size() < (size_t)capacity && !active.count(idx);
                bool inserted = queue.insert(idx);
                assert(inserted == room);
                if (inserted) active.insert(idx);
            } else if (operation == 1) {
                keys[idx] = int(random() % 1000) * 64 + idx;
                assert(queue.ChangePriority(idx) == (active.count(idx) != 0));
            } else {
                int idx = queue.Pop();
                if (active.empty()) assert(idx == -1);
                else {
                    auto best = std::min_element(active.begin(), active.end(), [&](int a, int b) {
                        return keys[a] < keys[b];
                    });
                    assert(idx == *best); active.erase(best);
                    assert(!queue.contains(idx) && !queue.ChangePriority(idx));
                }
            }
            assert(queue.empty() == active.empty());
        }
    }
    std::vector<int> shrinking = {2, 1};
    IndexedPriorityQLow<int> queue(shrinking, 2);
    assert(queue.insert(0) && queue.insert(1)); shrinking.clear();
    assert(queue.empty() && queue.Pop() == -1 && !queue.ChangePriority(1) && !queue.insert(0));
    using Graph = SparseGraph<NaviNode, NaviLink>;
    Graph graph(true), no_nodes(true);
    for (int i = 0; i < 5; ++i) graph.AddNode(NaviNode(i));
    graph.AddEdge(NaviLink(0, 1, 10)); graph.AddEdge(NaviLink(0, 2, 1));
    graph.AddEdge(NaviLink(2, 1, 1)); graph.AddEdge(NaviLink(1, 3, 2));
    for (int source : {-2147483647 - 1, -1, 0, 3, 5, 2147483647}) {
        for (int target : {-2147483647 - 1, -1, 0, 3, 4, 5, 2147483647}) {
            Graph_SearchDijkstra<Graph> dijkstra(graph, source, target);
            Graph_SearchAStar<Graph, Heuristic> astar(graph, source, target);
            bool valid = source >= 0 && source < 5 && target >= 0 && target < 5;
            if (!valid) assert(dijkstra.GetPathToTarget().empty() && astar.GetPathToTarget().empty());
            (void)dijkstra.GetCostToTarget(); (void)astar.GetCostToTarget();
            assert(dijkstra.GetCostToNode(0xffffffff) == 0);
        }
    }
    Graph_SearchDijkstra<Graph> best(graph, 0, 3), unreachable(graph, 0, 4), same(graph, 3, 3);
    Graph_SearchAStar<Graph, Heuristic> best_astar(graph, 0, 3);
    assert(best.GetPathToTarget() == std::list<int>({0, 2, 1, 3}) && best.GetCostToTarget() == 4);
    assert(best_astar.GetPathToTarget() == best.GetPathToTarget() && best_astar.GetCostToTarget() == 4);
    assert(unreachable.GetPathToTarget().empty() && same.GetPathToTarget() == std::list<int>({3}));
    assert(Graph_SearchDijkstra<Graph>(no_nodes, 0, 0).GetPathToTarget().empty());
    assert((Graph_SearchAStar<Graph, Heuristic>(no_nodes, 0, 0).GetPathToTarget().empty()));
    Graph_MinSpanningTree<Graph> tree(graph, 0), invalid_tree(graph, 2147483647);
    assert(tree.GetSpanningTree().size() == 5);
    for (auto edge : invalid_tree.GetSpanningTree()) assert(!edge);
    BadGraph malformed;
    Graph_SearchDijkstra<BadGraph> tolerant(malformed, 0, 2);
    Graph_SearchAStar<BadGraph, Heuristic> tolerant_astar(malformed, 0, 2);
    assert(tolerant.GetPathToTarget() == std::list<int>({0, 1, 2}));
    assert(tolerant_astar.GetPathToTarget() == tolerant.GetPathToTarget());
    puts("Navigation heap capacity, index validation and shortest-path regressions passed");
}
}
''')
    return "\n".join(parts)
