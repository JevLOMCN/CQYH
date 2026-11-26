"""Production sparse graph mutation, iterator and serialized-load regressions."""


def fixture():
    return r'''
struct FileNode {
    int index;
    explicit FileNode(int i = -1) : index(i) {}
    explicit FileNode(std::ifstream& input) : index(-1) { input >> index; }
    int Index() const { return index; }
    void SetIndex(int i) { index = i; }
    friend std::ostream& operator<<(std::ostream& out, const FileNode& node) { return out << node.index << '\n'; }
};
struct FileEdge {
    int from, to;
    double cost;
    FileEdge(int f, int t, double c = 1) : from(f), to(t), cost(c) {}
    explicit FileEdge(std::ifstream& input) : from(-1), to(-1), cost(0) { input >> from >> to >> cost; }
    int From() const { return from; }
    int To() const { return to; }
    void SetFrom(int i) { from = i; }
    void SetTo(int i) { to = i; }
    double Cost() const { return cost; }
    void SetCost(double c) { cost = c; }
    friend std::ostream& operator<<(std::ostream& out, const FileEdge& edge) {
        return out << edge.from << ' ' << edge.to << ' ' << edge.cost << '\n';
    }
};
void CheckSparseGraph() {
    using Graph = SparseGraph<NaviNode, NaviLink>;
    for (bool directed : {false, true}) {
        Graph graph(directed);
        for (int i = 0; i < 6; ++i) assert(graph.AddNode(NaviNode(i)) == i);
        assert(graph.AddNode(NaviNode(-1)) == -1 && graph.AddNode(NaviNode(20)) == -1);
        assert(graph.AddNode(NaviNode(0)) == -1 && graph.NumNodes() == 6);
        for (int invalid : {-2147483647 - 1, -1, 6, 2147483647}) {
            assert(!graph.isNodePresent(invalid));
            assert(!graph.isEdgePresent(invalid, 0) && !graph.isEdgePresent(0, invalid));
            graph.AddEdge(NaviLink(invalid, 0)); graph.AddEdge(NaviLink(0, invalid));
            graph.RemoveNode(invalid); graph.RemoveEdge(invalid, 0); graph.RemoveEdge(0, invalid);
            graph.SetEdgeCost(invalid, 0, 1); graph.SetEdgeCost(0, invalid, 1);
            Graph::EdgeIterator edges(graph, invalid);
            Graph::ConstEdgeIterator const_edges(graph, invalid);
            assert(edges.end() && !edges.begin() && !edges.next());
            assert(const_edges.end() && !const_edges.begin() && !const_edges.next());
            bool caught = false;
            try { graph.GetNode(invalid); } catch (const std::out_of_range&) { caught = true; }
            assert(caught); caught = false;
            try { static_cast<const Graph&>(graph).GetNode(invalid); }
            catch (const std::out_of_range&) { caught = true; }
            assert(caught); caught = false;
            try { graph.GetEdge(invalid, 0); } catch (const std::out_of_range&) { caught = true; }
            assert(caught); caught = false;
            try { static_cast<const Graph&>(graph).GetEdge(0, invalid); }
            catch (const std::out_of_range&) { caught = true; }
            assert(caught);
        }
        assert(graph.NumEdges() == 0);
        Graph::EdgeIterator none(graph, 0); Graph::ConstEdgeIterator const_none(graph, 0);
        assert(none.end() && !none.begin() && !none.next());
        assert(const_none.end() && !const_none.begin() && !const_none.next());
        graph.AddEdge(NaviLink(0, 0)); graph.AddEdge(NaviLink(0, 1));
        graph.AddEdge(NaviLink(0, 2)); graph.AddEdge(NaviLink(3, 0));
        graph.AddEdge(NaviLink(4, 0)); graph.AddEdge(NaviLink(5, 4, 8));
        Graph::EdgeIterator active_edges(graph, 0);
        int edge_count = 0;
        for (auto edge = active_edges.begin(); !active_edges.end(); edge = active_edges.next()) {
            assert(edge && edge->From() == 0); ++edge_count;
        }
        assert(edge_count == (directed ? 3 : 5) && !active_edges.next());
        assert(static_cast<const Graph&>(graph).GetEdge(5, 4).Cost() == 8);
        graph.SetEdgeCost(5, 4, 9); assert(graph.GetEdge(5, 4).Cost() == 9);
        graph.RemoveNode(0);
        assert(!graph.isNodePresent(0) && graph.NumActiveNodes() == 5);
        Graph::NodeIterator remaining(graph); Graph::ConstNodeIterator const_remaining(graph);
        int next_index = 1;
        for (auto node = remaining.begin(); !remaining.end(); node = remaining.next())
            assert(node && node->Index() == next_index++);
        assert(next_index == 6 && !remaining.next()); next_index = 1;
        for (auto node = const_remaining.begin(); !const_remaining.end(); node = const_remaining.next())
            assert(node && node->Index() == next_index++);
        assert(next_index == 6 && !const_remaining.next());
        assert(graph.NumEdges() == (directed ? 1 : 2));
        assert(graph.isEdgePresent(5, 4));
        for (int i = 0; i < 6; ++i) assert(!graph.isEdgePresent(i, 0) && !graph.isEdgePresent(0, i));
        graph.RemoveNode(0);
        assert(graph.AddNode(NaviNode(0)) == 0 && graph.GetNextFreeNodeIndex() == 6);
        graph.RemoveEdge(5, 4);
        assert(graph.NumEdges() == 0);
        bool caught = false;
        try { graph.GetEdge(5, 4); } catch (const std::runtime_error&) { caught = true; }
        assert(caught);
        for (int i = 0; i < 6; ++i) graph.RemoveNode(i);
        Graph::NodeIterator nodes(graph); Graph::ConstNodeIterator const_nodes(graph);
        assert(!nodes.begin() && nodes.end() && !nodes.next() && !nodes.next());
        assert(!const_nodes.begin() && const_nodes.end() && !const_nodes.next() && !const_nodes.next());
        graph.Clear();
        Graph::NodeIterator empty_nodes(graph); Graph::ConstNodeIterator empty_const_nodes(graph);
        assert(empty_nodes.end() && !empty_nodes.next() && !empty_nodes.begin());
        assert(empty_const_nodes.end() && !empty_const_nodes.next() && !empty_const_nodes.begin());
    }
    using FileGraph = SparseGraph<FileNode, FileEdge>;
    FileGraph loaded(true);
    loaded.AddNode(FileNode(0)); loaded.AddNode(FileNode(1)); loaded.AddEdge(FileEdge(0, 1, 7));
    auto load = [&](const char* text) {
        { std::ofstream file("graph-input.tmp", std::ios::binary | std::ios::trunc); file << text; }
        std::ifstream file("graph-input.tmp", std::ios::binary);
        return loaded.Load(file);
    };
    for (const char* invalid : {"", "x", "-1", "2147483648", "2147483647", "3 0", "1 -2 0",
          "2 0 0 0", "1 2 0", "1 0 -1", "1 0 1", "1 0 1 -1 0 2", "1 0 1 0 1 2",
          "1 -1 1 0 0 1", "2 0 1 2147483647 0 1 1"}) {
        assert(!load(invalid));
        assert(loaded.NumNodes() == 2 && loaded.NumEdges() == 1 && loaded.GetEdge(0, 1).Cost() == 7);
    }
    assert(!loaded.Load((const char*)nullptr) && !loaded.Save((const char*)nullptr));
    assert(load("3 0 -1 2 1 0 2 4.5"));
    assert(loaded.NumNodes() == 3 && loaded.NumActiveNodes() == 2 && !loaded.isNodePresent(1));
    assert(loaded.GetEdge(0, 2).Cost() == 4.5);
    assert(loaded.Save("graph-roundtrip.tmp"));
    FileGraph roundtrip(true); assert(roundtrip.Load("graph-roundtrip.tmp"));
    assert(roundtrip.NumNodes() == 3 && !roundtrip.isNodePresent(1) && roundtrip.GetEdge(0, 2).Cost() == 4.5);
    assert(load("2 0 1 3 1 0 1 1 0 2 1 0 3") && loaded.NumEdges() == 3);
    loaded.RemoveNode(0); assert(loaded.NumEdges() == 0);
    assert(load("0 0") && !loaded.NumNodes() && !loaded.NumEdges());
    remove("graph-input.tmp"); remove("graph-roundtrip.tmp");
    puts("Sparse graph bounds, iterator exhaustion, node removal and staged file-load regressions passed");
}
'''
