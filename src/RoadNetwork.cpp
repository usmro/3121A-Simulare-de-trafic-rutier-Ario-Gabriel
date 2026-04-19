#include "RoadNetwork.h"
#include "Vehicle.h"
#include <queue>
#include <map>
#include <climits>
#include <algorithm>

void RoadNetwork::build() {
    auto A = std::make_shared<Intersection>("A");
    auto B = std::make_shared<Intersection>("B");
    auto C = std::make_shared<Intersection>("C");
    auto D = std::make_shared<Intersection>("D");
    auto E = std::make_shared<Intersection>("E");
    auto F = std::make_shared<Intersection>("F");
    auto G = std::make_shared<Intersection>("G");
    auto H = std::make_shared<Intersection>("H");
    auto I = std::make_shared<Intersection>("I");

    intersections = {A, B, C, D, E, F, G, H, I};

    auto AB = std::make_shared<Street>("AB", A.get(), B.get(), 100, 50);
    auto BC = std::make_shared<Street>("BC", B.get(), C.get(), 100, 50);
    auto DE = std::make_shared<Street>("DE", D.get(), E.get(), 100, 50);
    auto EF = std::make_shared<Street>("EF", E.get(), F.get(), 100, 50);
    auto GH = std::make_shared<Street>("GH", G.get(), H.get(), 100, 50);
    auto HI = std::make_shared<Street>("HI", H.get(), I.get(), 100, 50);
    auto AD = std::make_shared<Street>("AD", A.get(), D.get(), 120, 60);
    auto DG = std::make_shared<Street>("DG", D.get(), G.get(), 120, 60);
    auto BE = std::make_shared<Street>("BE", B.get(), E.get(), 120, 60);
    auto EH = std::make_shared<Street>("EH", E.get(), H.get(), 120, 60);
    auto CF = std::make_shared<Street>("CF", C.get(), F.get(), 120, 60);
    auto FI = std::make_shared<Street>("FI", F.get(), I.get(), 120, 60);

    streets = {AB, BC, DE, EF, GH, HI, AD, DG, BE, EH, CF, FI};

    A->addStreet(AB.get()); A->addStreet(AD.get());
    B->addStreet(AB.get()); B->addStreet(BC.get()); B->addStreet(BE.get());
    C->addStreet(BC.get()); C->addStreet(CF.get());
    D->addStreet(AD.get()); D->addStreet(DE.get()); D->addStreet(DG.get());
    E->addStreet(DE.get()); E->addStreet(EF.get());
    E->addStreet(BE.get()); E->addStreet(EH.get());
    F->addStreet(EF.get()); F->addStreet(CF.get()); F->addStreet(FI.get());
    G->addStreet(DG.get()); G->addStreet(GH.get());
    H->addStreet(GH.get()); H->addStreet(HI.get()); H->addStreet(EH.get());
    I->addStreet(HI.get()); I->addStreet(FI.get());
}

Intersection* RoadNetwork::getIntersection(std::string id) {
    for (auto& i : intersections)
        if (i->getId() == id) return i.get();
    return nullptr;
}

std::vector<std::shared_ptr<Street>>& RoadNetwork::getStreets() {
    return streets;
}

std::vector<std::shared_ptr<Intersection>>& RoadNetwork::getIntersections() {
    return intersections;
}

std::vector<Street*> RoadNetwork::findPath(Intersection* start,
                                            Intersection* goal,
                                            Vehicle* vehicle) {
    using pii = std::pair<int, Intersection*>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

    std::map<Intersection*, int> dist;
    std::map<Intersection*, Street*> previous;

    pq.push({0, start});
    dist[start] = 0;

    while (!pq.empty()) {
        auto [cost, current] = pq.top();
        pq.pop();

        if (current == goal) break;

        for (Street* s : current->getStreets()) {
            Intersection* next = s->getOtherEnd(current);
            if (next == nullptr) continue;

            int newCost = cost + vehicle->edgeCost(s);

            if (dist.find(next) == dist.end() || newCost < dist[next]) {
                dist[next] = newCost;
                previous[next] = s;
                pq.push({newCost, next});
            }
        }
    }

    std::vector<Street*> path;
    Intersection* current = goal;

    while (previous.find(current) != previous.end()) {
        Street* s = previous[current];
        path.push_back(s);
        current = s->getOtherEnd(current);
    }

    std::reverse(path.begin(), path.end());
    return path;
}