#ifndef GA06_ORTOGONALNOKONVEKSNIPOLIGON_H
#define GA06_ORTOGONALNOKONVEKSNIPOLIGON_H

#include "algoritambaza.h"
#include <QPoint>
#include <set>
#include <QPainterPath>

enum Orientation {H, V};
enum NodeState {HH, HV, VH, VV};

struct Node
{
    int i1, i2;
    std::vector<std::vector<NodeState>> links;

    Node(int i, int j) : i1(i), i2(j), links({{},{},{},{}}) {}
};
using SolutionGraph = std::vector<Node>;
using Coords = std::pair<int, int>;

struct qPointComparator
{
    bool operator()(QPoint a, QPoint b) const
    {
        return a.x()==b.x() ? a.y() < b.y() : a.x() < b.x();
    }
};

const std::vector<NodeState> nodeStates = {HH, HV, VH, VV};

class PolygonCover : public AlgoritamBaza
{
public:
    PolygonCover(QWidget *pCrtanje,
                 int pauzaKoraka,
                 const bool &naivni = false,
                 std::string imeDatoteke = "",
                 int brojTacaka = BROJ_SLUCAJNIH_OBJEKATA);

    void pokreniAlgoritam() final;
    void crtajAlgoritam(QPainter *painter) const final;
    void pokreniNaivniAlgoritam() final;
    void crtajNaivniAlgoritam(QPainter *painter) const final;

private:
    static std::pair<Orientation, Orientation> stateToOrientations(NodeState state);
    static NodeState orientationsToState(Orientation o1, Orientation o2);
    static void flip(Orientation& o);
    static Orientation flipped(Orientation o);
    static void printPoints(std::vector<QPoint> points);
    static bool sortNW(QPoint a, QPoint b);
    static bool sortNE(QPoint a, QPoint b);
    static bool sortSE(QPoint a, QPoint b);
    static bool sortSW(QPoint a, QPoint b);
    static std::vector<NodeState> GOO(NodeState sState, int pointCount);



    void createCover();
    std::pair<std::vector<QPoint>, std::vector<QPoint>> separateNWSE(const std::vector<QPoint>& points);
    std::optional<NodeState> checkIntersection(Node start, Node target, NodeState sState, const std::vector<QPoint>& NW, const std::vector<QPoint>& SE);
    std::vector<NodeState> getIncomingLinks(const SolutionGraph& sg, int i, NodeState target);
    SolutionGraph createSolutionGraph(const std::vector<QPoint>& NW, const std::vector<QPoint>& SE);
    std::pair<std::vector<QPoint>, std::vector<QPoint>> separateRegion(int nw1, int se1, int nw2, NodeState incomingState, NodeState outgoingState, const std::vector<QPoint>& NW, const std::vector<QPoint>& SE);
    std::pair<std::vector<QPoint>, std::vector<QPoint>> createNWSE(const std::vector<QPoint>& points);
    void getFinalPolygon(const std::vector<QPoint>& points);
    void drawOverlaps();

    QList<QPoint> _polygon;
    std::vector<QPoint> _points;
    int _quadrantsConfiguration;
    QPoint _N, _E, _S, _W;
    std::vector<QPoint> _pointsNW, _pointsNE, _pointsSE, _pointsSW;
    std::vector<QPoint> _stairsNW, _stairsNE, _stairsSE, _stairsSW;
    bool _infeasible;
    std::map<Coords, bool> _used;
    std::map<Coords, bool> _overlap;
    bool _drawQuadrants;

};




#endif // GA06_ORTOGONALNOKONVEKSNIPOLIGON_H
