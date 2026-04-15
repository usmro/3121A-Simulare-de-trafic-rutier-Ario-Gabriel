#include "RoadNetwork.h"

void RoadNetwork::build() {
    // ── STEP 1: Create all 9 intersections ──
    // We use make_shared so memory is managed automatically
    // The string is the ID we'll use to find them later

    auto A = std::make_shared<Intersection>("A");
    auto B = std::make_shared<Intersection>("B");
    auto C = std::make_shared<Intersection>("C");
    auto D = std::make_shared<Intersection>("D");
    auto E = std::make_shared<Intersection>("E");
    auto F = std::make_shared<Intersection>("F");
    auto G = std::make_shared<Intersection>("G");
    auto H = std::make_shared<Intersection>("H");
    auto I = std::make_shared<Intersection>("I");

    // store them so we can find them later
    intersections = {A, B, C, D, E, F, G, H, I};

    // ── STEP 2: Create all 12 streets ──
    // Street(name, endA, endB, length, speedLimit)
    // .get() converts shared_ptr to raw pointer

    // top row:    A-B-C
    auto AB = std::make_shared<Street>("AB", A.get(), B.get(), 100, 50);
    auto BC = std::make_shared<Street>("BC", B.get(), C.get(), 100, 50);

    // middle row: D-E-F
    auto DE = std::make_shared<Street>("DE", D.get(), E.get(), 100, 50);
    auto EF = std::make_shared<Street>("EF", E.get(), F.get(), 100, 50);

    // bottom row: G-H-I
    auto GH = std::make_shared<Street>("GH", G.get(), H.get(), 100, 50);
    auto HI = std::make_shared<Street>("HI", H.get(), I.get(), 100, 50);

    // left column:   A-D-G
    auto AD = std::make_shared<Street>("AD", A.get(), D.get(), 120, 60);
    auto DG = std::make_shared<Street>("DG", D.get(), G.get(), 120, 60);

    // middle column: B-E-H
    auto BE = std::make_shared<Street>("BE", B.get(), E.get(), 120, 60);
    auto EH = std::make_shared<Street>("EH", E.get(), H.get(), 120, 60);

    // right column:  C-F-I
    auto CF = std::make_shared<Street>("CF", C.get(), F.get(), 120, 60);
    auto FI = std::make_shared<Street>("FI", F.get(), I.get(), 120, 60);

    streets = {AB, BC, DE, EF, GH, HI, AD, DG, BE, EH, CF, FI};

    // ── STEP 3: Connect streets to intersections ──
    // Every intersection needs to know which streets touch it
    // This is what makes the graph traversable

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
