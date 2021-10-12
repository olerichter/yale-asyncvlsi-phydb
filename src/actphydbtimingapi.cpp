//
// Created by yihang on 4/18/21.
//

#include "actphydbtimingapi.h"

#include "logging.h"

namespace phydb {

PhydbTimingEdge *PhydbTimingNode::AddPinToOutEdges(PhydbPin &pin) {
    PhyDBExpects(!IsTargetPinExisting(pin),
                 "Pin in OutEdges already, cannot add it again!" + source.Str()
                     + pin.Str());
    int id = (int) out_edges.size();
    out_edges.emplace_back(pin);
    out_pin2index_[pin] = id;
    return &out_edges[id];
}

PhydbTimingEdge *PhydbTimingNode::GetOutEdge(PhydbPin &pin) {
    PhyDBExpects(IsTargetPinExisting(pin),
                 "Pin not in OutEdges, cannot find it!" + source.Str()
                     + pin.Str());
    int id = out_pin2index_[pin];
    return &out_edges[id];
}

void PhydbTimingNode::AddEdge(PhydbTimingEdge &edge) {
    PhydbPin pin = edge.target;
    PhydbTimingEdge *edge_ptr = nullptr;
    if (IsTargetPinExisting(pin)) {
        edge_ptr = GetOutEdge(pin);
        PhyDBExpects((edge_ptr->delay == edge.delay)
                         && (edge_ptr->net_index == edge.net_index),
                     "Data value inconsistency during adding edge");
        edge_ptr->count += edge.count;
    } else {
        edge_ptr = AddPinToOutEdges(pin);
        edge_ptr->net_index = edge.net_index;
        edge_ptr->delay = edge.delay;
        edge_ptr->count = edge.count;
    }
}

void PhydbPath::Clear() {
    edges.clear();
    root.Reset();
}

void PhydbPath::AddEdge(
    PhydbPin &src,
    PhydbPin &tgt,
    int net_id,
    double dly,
    int cnt
) {
    if (edges.empty()) {
        root = src;
    } else {
        PhyDBExpects(src == edges.back().target,
                     "This edge does not form a linked list?");
    }
    PhydbTimingEdge &edge = edges.emplace_back(tgt);
    edge.net_index = net_id;
    edge.delay = dly;
    edge.count = cnt;
}

PhydbTimingNode *TimingDAG::AddPinToDag(PhydbPin &pin) {
    PhyDBExpects(!IsPinInDag(pin),
                 "Pin in DAG already, cannot add it again!" + pin.Str());
    int id = (int) stb_fast_nodes.size();
    stb_fast_nodes.emplace_back(pin);
    fast_pin2index_[pin] = id;
    return &stb_fast_nodes[id];
}

PhydbTimingNode *TimingDAG::GetPinNode(PhydbPin &pin) {
    PhyDBExpects(IsPinInDag(pin),
                 "Pin not in DAG, cannot find it!" + pin.Str());
    int id = fast_pin2index_[pin];
    return &stb_fast_nodes[id];
}

void TimingDAG::AddFastPath(PhydbPath &fast_path) {
    PhydbTimingNode *node = nullptr;
    if (!IsPinInDag(fast_path.root)) {
        node = AddPinToDag(fast_path.root);
    } else {
        node = GetPinNode(fast_path.root);
    }
    for (auto &edge: fast_path.edges) {
        node->AddEdge(edge);
        PhydbPin target = edge.target;
        node = GetPinNode(target);
    }
}

bool ActPhyDBTimingAPI::IsActNetPtrExisting(void *act_net) {
    return net_act_2_id_.find(act_net) != net_act_2_id_.end();
}

int ActPhyDBTimingAPI::ActNetPtr2Id(void *act_net) {
    if (IsActNetPtrExisting(act_net)) {
        return net_act_2_id_[act_net];
    }
    return -1;
}

void *ActPhyDBTimingAPI::PhydbNetId2ActPtr(int net_id) {
    auto ret = net_id_2_act_.find(net_id);
    if (ret != net_id_2_act_.end()) {
        return ret->second;
    }
    return nullptr;
}

void ActPhyDBTimingAPI::AddActNetPtrIdPair(void *act_net, int net_id) {
    PhyDBExpects(!IsActNetPtrExisting(act_net),
                 "Cannot add ACT net again, it is in already in the PhyDB, net id: "
                     + std::to_string(net_id));
    std::pair<void *, int> tmp_pair_0(act_net, net_id);
    std::pair<int, void *> tmp_pair_1(net_id, act_net);
    net_act_2_id_.insert(tmp_pair_0);
    net_id_2_act_.insert(tmp_pair_1);
}

void ActPhyDBTimingAPI::AddActCompPinPtrIdPair(void *act_pin,
                                               PhydbPin phydb_pin) {
    PhyDBExpects(!IsActComPinPtrExisting(act_pin),
                 "Cannot add ACT pin again, it is in already in the PhyDB, component pin id: "
                     + phydb_pin.Str());
    std::pair<void *, PhydbPin> tmp_pair_0(act_pin, phydb_pin);
    std::pair<PhydbPin, void *> tmp_pair_1(phydb_pin, act_pin);
    component_pin_act_2_id_.insert(tmp_pair_0);
    component_pin_id_2_act_.insert(tmp_pair_1);
}

void ActPhyDBTimingAPI::AddActCompPinPtrIdPair(
    void *act_pin,
    int comp_id,
    int pin_id
) {
    PhydbPin phydb_pin(comp_id, pin_id);
    AddActCompPinPtrIdPair(act_pin, phydb_pin);
}

bool ActPhyDBTimingAPI::IsActComPinPtrExisting(void *act_pin) {
    return component_pin_act_2_id_.find(act_pin)
        != component_pin_act_2_id_.end();
}

PhydbPin ActPhyDBTimingAPI::ActCompPinPtr2Id(void *act_pin) {
    if (IsActComPinPtrExisting(act_pin)) {
        return component_pin_act_2_id_[act_pin];
    }
    return PhydbPin(-1, -1);
}

void *ActPhyDBTimingAPI::PhydbCompPin2ActPtr(PhydbPin phydb_pin) {
    auto ret = component_pin_id_2_act_.find(phydb_pin);
    if (ret != component_pin_id_2_act_.end()) {
        return ret->second;
    }
    return nullptr;
}

void ActPhyDBTimingAPI::SetGetNumConstraintsCB(int (*callback_function)()) {
    GetNumConstraintsCB = callback_function;
}

void ActPhyDBTimingAPI::SetUpdateTimingIncrementalCB(void (*callback_function)()) {
    UpdateTimingIncrementalCB = callback_function;
}

void ActPhyDBTimingAPI::SetGetSlackCB(std::vector<double> (*callback_function)(
    const std::vector<int> &)) {
    GetSlackCB = callback_function;
}

void ActPhyDBTimingAPI::SetGetWitnessCB(
    void (*callback_function)(
        int,
        std::vector<ActEdge> &,
        std::vector<ActEdge> &
    )
) {
    GetWitnessCB = callback_function;
}

void ActPhyDBTimingAPI::SetGetViolatedTimingConstraintsCB(
    void (*callback_function)(std::vector<int> &)
) {
    GetViolatedTimingConstraintsCB = callback_function;
}
#if PHYDB_USE_GALOIS
void ActPhyDBTimingAPI::SetParaManager(galois::eda::parasitics::Manager *manager) {
    para_manager_ = manager;
}
void ActPhyDBTimingAPI::AddCellLib(galois::eda::liberty::CellLib *lib) {
    libs_.push_back(lib);
}
void ActPhyDBTimingAPI::SetNetlistAdaptor(galois::eda::utility::ExtNetlistAdaptor *adaptor) {
    adaptor_ = adaptor;
}

galois::eda::parasitics::Manager *ActPhyDBTimingAPI::GetParaManager() {
    return para_manager_;
}

std::vector<galois::eda::liberty::CellLib *> &ActPhyDBTimingAPI::GetCellLibs() {
    return libs_;
}

galois::eda::utility::ExtNetlistAdaptor *ActPhyDBTimingAPI::GetNetlistAdaptor() {
    return adaptor_;
}

galois::eda::parasitics::Node *ActPhyDBTimingAPI::PhyDBPinToSpefNode(PhydbPin phydb_pin) {
    void *act_pin = PhydbCompPin2ActPtr(phydb_pin);
    PhyDBExpects(act_pin != nullptr, "Cannot map PhyDB pin to ACT?");
    auto node = para_manager_->findPin(act_pin);
    PhyDBExpects(node != nullptr, "Cannot map ACT pin to SPEF node?");
    return node;
}

#endif

int ActPhyDBTimingAPI::GetNumConstraints() {
    PhyDBExpects(GetNumConstraintsCB != nullptr,
                 "Callback function for GetNumConstraints() is not set");
    return GetNumConstraintsCB();
}

void ActPhyDBTimingAPI::UpdateTimingIncremental() {
    PhyDBExpects(UpdateTimingIncrementalCB != nullptr,
                 "Callback function for UpdateTimingIncremental() is not set");
    UpdateTimingIncrementalCB();
}

double ActPhyDBTimingAPI::GetSlack(int tc_num) {
    PhyDBExpects(GetSlackCB != nullptr,
                 "Callback function for GetSlack() is not set");
    return GetSlackCB(std::vector(1, tc_num))[0];
}

void ActPhyDBTimingAPI::GetWitness(
    int tc_num,
    PhydbPath &phydb_fast_path,
    PhydbPath &phydb_slow_path
) {
    PhyDBExpects(GetWitnessCB != nullptr,
                 "Callback GetWitness for GetSlack() is not set");
    std::vector<ActEdge> act_fast_path;
    std::vector<ActEdge> act_slow_path;

    GetWitnessCB(tc_num, act_fast_path, act_slow_path);

    TranslateActPathToPhydbPath(act_fast_path, phydb_fast_path);
    TranslateActPathToPhydbPath(act_slow_path, phydb_slow_path);
}

void ActPhyDBTimingAPI::GetViolatedTimingConstraints(std::vector<int> &violated_tc_nums) {
    PhyDBExpects(GetViolatedTimingConstraintsCB != nullptr,
                 "Callback GetWitness for GetViolatedTimingConstraints() is not set");
    GetViolatedTimingConstraintsCB(violated_tc_nums);
}

void ActPhyDBTimingAPI::TranslateActPathToPhydbPath(
    std::vector<ActEdge> &act_path,
    PhydbPath &phydb_path
) {
    phydb_path.Clear();

    size_t sz = act_path.size();

    for (size_t i = 0; i < sz; ++i) {
        ActEdge &act_edge = act_path[i];
        PhyDBExpects(IsActComPinPtrExisting(act_edge.source),
                     "Cannot translate ActEdge to PhydbEdge, ActEdge not in the database");
        PhyDBExpects(IsActComPinPtrExisting(act_edge.target),
                     "Cannot translate ActEdge to PhydbEdge, ActEdge not in the database");
        PhyDBExpects(IsActNetPtrExisting(act_edge.net_ptr),
                     "Cannot translate ActEdge to PhydbEdge, ActEdge not in the database");
        PhyDBExpects(act_edge.delay >= 0, "Negative delay?");

        if (i == 0) {
            phydb_path.root = ActCompPinPtr2Id(act_edge.source);
        }

        PhydbPin source = ActCompPinPtr2Id(act_edge.source);
        PhydbPin target = ActCompPinPtr2Id(act_edge.target);
        int net_index = ActNetPtr2Id(act_edge.net_ptr);
        double delay = act_edge.delay;
        phydb_path.AddEdge(source, target, net_index, delay, 1);
    }
}

}
