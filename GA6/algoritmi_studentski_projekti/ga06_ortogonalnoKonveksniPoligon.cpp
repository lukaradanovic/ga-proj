#include "ga06_ortogonalnoKonveksniPoligon.h"

PolygonCover::PolygonCover(QWidget *pCrtanje,
                           int pauzaKoraka,
                           const bool &naivni,
                           std::string imeDatoteke,
                           int brojTacaka)
    : AlgoritamBaza(pCrtanje, pauzaKoraka, naivni), _polygon({}), _points({}), _quadrantsConfiguration(-1),
    _N(QPoint(0,0)), _E(QPoint(0,0)), _S(QPoint(0,0)), _W(QPoint(0,0)),
    _pointsNW({}), _pointsNE({}), _pointsSE({}), _pointsSW({}), _stairsNW({}), _stairsNE({}), _stairsSE({}), _stairsSW({}), _infeasible(false), _used({}), _overlap({}), _drawQuadrants(false)
{
    if (imeDatoteke == "")
        _points = {{-5,0}, {0, 5}, {5, 0}, {0, -5}};//generisiNasumicneTacke(brojTacaka);
    else
        _points = ucitajPodatkeIzDatoteke(imeDatoteke);
}

void PolygonCover::pokreniAlgoritam()
{
    createCover();


    emit animacijaZavrsila();
}


void PolygonCover::crtajAlgoritam(QPainter *painter) const
{
    if (!painter) return;

    QColor colorDefault = Qt::black;
    QColor colorNW = QColor(215, 55, 21);
    QColor colorSE = QColor(18, 130, 227);
    QColor colorNE = QColor(242, 207, 67);
    QColor colorSW = QColor(78, 161, 81);
    QColor colorNWSE = QColor(235, 135, 35); //174, 86, 184
    QColor colorNESW = QColor(95, 204, 211); //85, 194, 201
    QColor colorOverlap = QColor(175, 66, 212);
    QColor colorPoly = QColor(100, 100, 100);//QColor(100, 100, 100);//QColor(189, 190, 191);
    QColor colorBorder = QColor(220, 220, 220);
    int widthPoint = 12;
    int widthPoly = 3;
    int widthBorder = 2;


    QPen p = painter->pen();
    p.setColor(colorDefault);
    p.setWidth(widthPoint);
    p.setCapStyle(Qt::RoundCap);




    painter->setPen(p);


    if (_polygon.size() > 0)
    {
        p.setWidth(widthPoly);
        p.setColor(colorPoly);
        painter->setPen(p);
        QPainterPath path(_polygon.front());
        for (size_t i = 1; i < _polygon.size(); ++i)
            path.lineTo(_polygon[i]);
        painter->drawPath(path);
        p.setWidth(widthPoint);
    }

    if (_drawQuadrants)// && _polygon.size() == 0)
    {
        p.setColor(colorBorder);
        p.setWidth(widthBorder);
        painter->setPen(p);
        QPaintDevice * device = painter->device();
        painter->drawLine(QPoint(_N.x(), device->height()), QPoint(_N.x(), std::min(_W.y(), _E.y())));
        painter->drawLine(QPoint(_S.x(), 0), QPoint(_S.x(), std::max(_W.y(), _E.y())));
        painter->drawLine(QPoint(0, _W.y()), QPoint(std::max(_N.x(), _S.x()), _W.y()));
        painter->drawLine(QPoint(device->width(), _E.y()), QPoint(std::min(_N.x(), _S.x()), _E.y()));

        p.setWidth(widthPoint);
    }


    if (_quadrantsConfiguration < 0)
    {
        p.setColor(colorDefault);
        painter->setPen(p);
        for (auto &point : _points)
            painter->drawPoint(point);
    }
    else if (_quadrantsConfiguration == 3)
    {

        p.setColor(colorDefault);
        painter->setPen(p);
        painter->drawPoint(_N);
        painter->drawPoint(_S);
        painter->drawPoint(_W);
        painter->drawPoint(_E);

        p.setColor(colorNW);
        painter->setPen(p);
        for (auto &point : _pointsNW)
            painter->drawPoint(point);

        p.setColor(colorNE);
        painter->setPen(p);
        for (auto &point : _pointsNE)
            painter->drawPoint(point);

        p.setColor(colorSE);
        painter->setPen(p);
        for (auto &point : _pointsSE)
            painter->drawPoint(point);

        p.setColor(colorSW);
        painter->setPen(p);
        for (auto &point : _pointsSW)
            painter->drawPoint(point);
    }
    else if (_quadrantsConfiguration == 1)
    {
        p.setColor(colorNE);
        painter->setPen(p);
        for (auto &point : _pointsNE)
            painter->drawPoint(point);


        p.setColor(colorSW);
        painter->setPen(p);
        for (auto &point : _pointsSW)
            painter->drawPoint(point);

        if (_stairsNW.size() == 0)
        {
            p.setColor(colorDefault);
            painter->setPen(p);
            painter->drawPoint(_N);
            painter->drawPoint(_E);
            painter->drawPoint(_S);
            painter->drawPoint(_W);


            p.setColor(colorNWSE);
            painter->setPen(p);
            for (auto &point : _pointsNW)
                painter->drawPoint(point);
            for (auto &point : _pointsSE)
                painter->drawPoint(point);


            if (_overlap.size() > 0)
            {
                p.setColor(colorOverlap);
                painter->setPen(p);
                for (auto it = _overlap.begin(); it != _overlap.end(); it++)
                {
                    if (it->second)
                    {
                        auto coords = it->first;
                        painter->drawPoint(QPoint(coords.first, coords.second));
                    }
                }
            }


        }
        else
        {
            p.setColor(colorNW);
            painter->setPen(p);
            for (auto &point : _stairsNW)
                painter->drawPoint(point);

            p.setColor(colorSE);
            painter->setPen(p);
            for (auto &point : _stairsSE)
                painter->drawPoint(point);

        }

    }
    else if (_quadrantsConfiguration == 2)
    {
        p.setColor(colorNW);
        painter->setPen(p);
        for (auto &point : _pointsNW)
            painter->drawPoint(point);


        p.setColor(colorSE);
        painter->setPen(p);
        for (auto &point : _pointsSE)
            painter->drawPoint(point);

        if (_stairsNE.size() == 0)
        {
            p.setColor(colorDefault);
            painter->setPen(p);
            painter->drawPoint(_N);
            painter->drawPoint(_E);
            painter->drawPoint(_S);
            painter->drawPoint(_W);


            p.setColor(colorNESW);
            painter->setPen(p);
            for (auto &point : _pointsNE)
                painter->drawPoint(point);
            for (auto &point : _pointsSW)
                painter->drawPoint(point);


            if (_overlap.size() > 0)
            {
                p.setColor(colorOverlap);
                painter->setPen(p);
                for (auto it = _overlap.begin(); it != _overlap.end(); it++)
                {
                    if (it->second)
                    {
                        auto coords = it->first;
                        painter->drawPoint(QPoint(coords.first, coords.second));
                    }
                }
            }


        }
        else
        {
            p.setColor(colorNE);
            painter->setPen(p);
            for (auto &point : _stairsNE)
                painter->drawPoint(point);

            p.setColor(colorSW);
            painter->setPen(p);
            for (auto &point : _stairsSW)
                painter->drawPoint(point);
        }

    }

}


void PolygonCover::pokreniNaivniAlgoritam()
{
    emit animacijaZavrsila();
}

void PolygonCover::crtajNaivniAlgoritam(QPainter *painter) const
{
    if (!painter) return;
}


void PolygonCover::createCover()
{
    _polygon.clear();
    _pointsNW.clear();
    _pointsNE.clear();
    _pointsSE.clear();
    _pointsSW.clear();
    _stairsNW.clear();
    _stairsNE.clear();
    _stairsSE.clear();
    _stairsSW.clear();
    _infeasible = false;
    _used.clear();
    _overlap.clear();
    _drawQuadrants = false;

    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();



    _N = _points[0], _E = _points[0], _S = _points[0], _W = _points[0];

    for (int i = 0; i < _points.size(); i++)
    {
        if (_points[i].y() > _N.y())
            _N = _points[i];
        if (_points[i].y() < _S.y())
            _S = _points[i];
        if (_points[i].x() > _E.x())
            _E = _points[i];
        if (_points[i].x() < _W.x())
            _W = _points[i];
    }

    _drawQuadrants = true;

    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();

    //int quadrantsConfiguration = 3;
    _quadrantsConfiguration = 3;

    for (int i = 0; i < _points.size(); i++)
    {
        if (_points[i] == _N || _points[i] == _S || _points[i] == _W || _points[i] == _E)
            continue;
        if (_points[i].x() <= _N.x() && _points[i].y() >= _W.y())
        {
            _pointsNW.push_back(_points[i]);
            if (_points[i].x() >= _S.x() && _points[i].y() <= _E.y())
                _quadrantsConfiguration = 1;
        }
        if (_points[i].x() >= _S.x() && _points[i].y() <= _E.y())
        {
            _pointsSE.push_back(_points[i]);
        }

        if (_points[i].x() >= _N.x() && _points[i].y() >= _E.y())
        {
            _pointsNE.push_back(_points[i]);
            if (_points[i].x() <= _S.x() && _points[i].y() <= _W.y())
                _quadrantsConfiguration = 2;
        }
        if (_points[i].x() <= _S.x() && _points[i].y() <= _W.y())
        {
            _pointsSW.push_back(_points[i]);
        }

    }

    qInfo() << "N: " << _N << "E: " << _E << "S: " << _S << "W: " << _W;
    qInfo() << "quadrants configuration: " << _quadrantsConfiguration;
    qInfo() << "quadrants sizes: " << "NW: " << _pointsNW.size() << "NE: " << _pointsNE.size() << "SE: " << _pointsSE.size() << "SW: " << _pointsSW.size();


    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();

    std::vector<QPoint> cover;


    if (_quadrantsConfiguration == 3)
    {
        std::sort(_pointsNW.begin(), _pointsNW.end(), sortNW);
        std::sort(_pointsNE.begin(), _pointsNE.end(), sortNE);
        std::sort(_pointsSE.begin(), _pointsSE.end(), sortSE);
        std::sort(_pointsSW.begin(), _pointsSW.end(), sortSW);

        if (_pointsNW.size() % 2 != 0 || _pointsNE.size() % 2 != 0 || _pointsSE.size() % 2 != 0 || _pointsSW.size() % 2 != 0)
        {
            qInfo() << "polygon construction impossible!";
            return;
        }

        cover.push_back(_W);
        for (auto& p : _pointsNW)
            cover.push_back(p);
        cover.push_back(_N);
        for (auto& p : _pointsNE)
            cover.push_back(p);
        cover.push_back(_E);
        for (auto& p : _pointsSE)
            cover.push_back(p);
        cover.push_back(_S);
        for (auto& p : _pointsSW)
            cover.push_back(p);


    }
    else if (_quadrantsConfiguration == 1)
    {
        std::vector<QPoint> NWSE = _pointsNW;
        for (auto& p : _pointsSE)
            NWSE.push_back(p);
        NWSE.push_back(_W);
        NWSE.push_back(_N);
        NWSE.push_back(_E);
        NWSE.push_back(_S);

        std::set<QPoint, qPointComparator> uniques(NWSE.begin(), NWSE.end());
        NWSE.assign(uniques.begin(), uniques.end());


        auto [stairsNW, stairsSE] = createNWSE(NWSE);

        if (_infeasible)
            return;

        for (auto& p : stairsNW)
                _stairsNW.push_back(p);
        for (auto& p : stairsSE)
                _stairsSE.push_back(p);


        if (_stairsNW.size() % 2 != 0 || _pointsNE.size() % 2 != 0 || _stairsSE.size() % 2 != 0 || _pointsSW.size() % 2 != 0)
        {
            qInfo() << "polygon construction impossible!";
            return;
        }


        AlgoritamBaza_updateCanvasAndBlock();
        AlgoritamBaza_updateCanvasAndBlock();
        AlgoritamBaza_updateCanvasAndBlock();

        std::sort(_pointsNE.begin(), _pointsNE.end(), sortNE);
        std::sort(_pointsSW.begin(), _pointsSW.end(), sortSW);
        std::sort(_stairsSE.begin(), _stairsSE.end(), sortSE);


        for (auto& p : _stairsNW)
            cover.push_back(p);
        for (auto& p : _pointsNE)
            cover.push_back(p);
        for (auto& p : _stairsSE)
            cover.push_back(p);
        for (auto& p : _pointsSW)
            cover.push_back(p);

    }
    else
    {
        std::vector<QPoint> NESW = _pointsNE;
        for (auto& p : _pointsSW)
            NESW.push_back(p);
        NESW.push_back(_W);
        NESW.push_back(_N);
        NESW.push_back(_E);
        NESW.push_back(_S);

        std::set<QPoint, qPointComparator> uniques(NESW.begin(), NESW.end());
        NESW.assign(uniques.begin(), uniques.end());

        for (int i = 0; i < NESW.size(); i++)
            NESW[i].setX(-NESW[i].x());

        auto [stairsNE, stairsSW] = createNWSE(NESW);

        if (_infeasible)
            return;


        for (auto& p : stairsNE)
                _stairsNE.push_back(p);
        for (auto& p : stairsSW)
                _stairsSW.push_back(p);


        for (int i = 0; i < _stairsNE.size(); i++)
            _stairsNE[i].setX(-_stairsNE[i].x());
        for (int i = 0; i < _stairsSW.size(); i++)
            _stairsSW[i].setX(-_stairsSW[i].x());


        if (_pointsNW.size() % 2 != 0 || _stairsNE.size() % 2 != 0 || _pointsSE.size() % 2 != 0 || _stairsSW.size() % 2 != 0)
        {
            qInfo() << "polygon construction impossible!";
            return;
        }


        AlgoritamBaza_updateCanvasAndBlock();
        AlgoritamBaza_updateCanvasAndBlock();
        AlgoritamBaza_updateCanvasAndBlock();

        std::sort(_pointsNW.begin(), _pointsNW.end(), sortNW);
        std::sort(_pointsSE.begin(), _pointsSE.end(), sortSE);
        std::sort(_stairsNE.begin(), _stairsNE.end(), sortNE);
        std::sort(_stairsSW.begin(), _stairsSW.end(), sortSW);

        // cover is expected to start from the west point
        cover.push_back(_stairsSW[_stairsSW.size()-1]);
        //_stairsSW.pop_back();

        for (auto& p : _pointsNW)
            cover.push_back(p);
        for (auto& p : _stairsNE)
            cover.push_back(p);
        for (auto& p : _pointsSE)
            cover.push_back(p);
        /*for (auto& p : _stairsSW)
            cover.push_back(p);*/
        for (int i = 0; i < _stairsSW.size()-1; i++)
            cover.push_back(_stairsSW[i]);
    }

    //printPoints(getFinalPolygon(cover));
    getFinalPolygon(cover);

    AlgoritamBaza_updateCanvasAndBlock();

}


std::pair<std::vector<QPoint>, std::vector<QPoint>> PolygonCover::separateNWSE(const std::vector<QPoint>& points)
{
    std::vector<QPoint> Sx, Sy, NW, SE;

    for (auto& p : points)
    {
        Sx.push_back(p);
        Sy.push_back(p);
        _used[{p.x(), p.y()}] = false;
        _overlap[{p.x(), p.y()}] = false;
    }
    std::sort(Sx.begin(), Sx.end(), [](QPoint a, QPoint b){ if (a.x() == b.x()) return a.y() > b.y(); else return a.x() < b.x();});
    std::sort(Sy.begin(), Sy.end(), [](QPoint a, QPoint b){ if (a.y() == b.y()) return a.x() > b.x(); else return a.y() < b.y();});

    NW.push_back(Sx[0]);
    SE.push_back(Sy[0]);
    _used[{Sx[0].x(), Sx[0].y()}] = true;
    _used[{Sy[0].x(), Sy[0].y()}] = true;


    for (int i = 1; i < Sx.size(); i++)
    {
        if (Sx[i].y() > NW[NW.size()-1].y())
        {
            NW.push_back(Sx[i]);
            _used[{Sx[i].x(), Sx[i].y()}] = true;
        }
        if (Sy[i].x() > SE[SE.size()-1].x())
        {
            SE.push_back(Sy[i]);
            _used[{Sy[i].x(), Sy[i].y()}] = true;
        }
        if (Sx[i] == Sy[i] && _used[{Sx[i].x(), Sx[i].y()}] && _used[{Sy[i].x(), Sy[i].y()}])
        {
            _overlap[{Sx[i].x(), Sx[i].y()}] = true;
            _overlap[{Sy[i].x(), Sy[i].y()}] = true;
        }
    }

    for (auto& p : points)
    {
        if (!_used[{p.x(), p.y()}])
        {
            qInfo() << "Infeasible!!";
            _infeasible = true;
            return {{},{}};
            //emit animacijaZavrsila();
        }
    }


    return {NW, SE};
}


std::optional<NodeState> PolygonCover::checkIntersection(Node start, Node target, NodeState sState, const std::vector<QPoint>& NW, const std::vector<QPoint>& SE)
{
    auto [upperO, lowerO] = stateToOrientations(sState);
    int u = start.i1, l = start.i2;
    if (u < target.i1 && l < target.i2 && sState == VH)
    {
        u++;
        flip(upperO);
    }

    while (u < target.i1 && l < target.i2)
    {
        if (upperO == H)
        {
            while (NW[u].y() > SE[l].y() && l < target.i2)
            {
                l++;
                flip(lowerO);
            }
            if (NW[u].y() > SE[l].y())
                break;
            else
            {
                if (lowerO == V && SE[l].x() <= NW[u+1].x())
                    return {};
                if (lowerO == H)
                {
                    if (SE[l-1].x() <= NW[u+1].x())
                        return {};
                    else
                    {
                        l--;
                        flip(lowerO);
                    }
                }
                u++;
                flip(upperO);
            }
        }
        else
        {
            while (NW[u].x() < SE[l].x() && u < target.i1)
            {
                u++;
                flip(upperO);
            }
            if (NW[u].x() < SE[l].x())
                break;
            else
            {
                if (upperO == V)
                {
                    if (SE[l+1].y() >= NW[u-1].y())
                        return {};
                    else
                    {
                        u--;
                        flip(upperO);
                    }
                }
                if (upperO == H && SE[l+1].y() >= NW[u].y())
                    return {};
                l++;
                flip(lowerO);
            }
        }
    }

    if (u == target.i1 && l == target.i2)
    {
        if (upperO == H && lowerO == H && NW[u].y() == SE[l].y())
            return {};
        if (upperO == V && lowerO == V && NW[u].x() == SE[l].x())
            return {};
        return orientationsToState(upperO, lowerO);
    }

    if (l == target.i2)
    {
        if (upperO == H && lowerO == H && !(NW[u].y() > SE[l].y()))
            return {};
        if (((upperO == V && lowerO == V) || (upperO == H && lowerO == V)) && !(NW[target.i1].x() < SE[l].x()))
            return {};
        if ((target.i1 - u) % 2)
            flip(upperO);
    }

    if (u == target.i1)
    {
        if (((upperO == H && lowerO == H) || (upperO == H && lowerO == V)) && !(NW[u].y() > SE[target.i2].y()))
            return {};
        if (upperO == V && lowerO == V && !(NW[u].x() < SE[l].x()))
            return {};
        if ((target.i2 - l) % 2)
            flip(lowerO);
    }

    return orientationsToState(upperO, lowerO);
}

std::vector<NodeState> PolygonCover::getIncomingLinks(const SolutionGraph& sg, int i, NodeState target)
{
    std::vector<NodeState> states;
    for (auto state : nodeStates)
    {
        auto stateLinks = sg[i-1].links[state];
        if (std::find(stateLinks.begin(), stateLinks.end(), target) != stateLinks.end())
            states.push_back(state);
    }
    return states;
}

SolutionGraph PolygonCover::createSolutionGraph(const std::vector<QPoint>& NW, const std::vector<QPoint>& SE)
{
    SolutionGraph graph;

    if (NW[0] == SE[0] || NW[NW.size()-1] == SE[SE.size()-1])
    {
        qInfo() << "Infeasible!!!";
        _infeasible = true;
        return graph;
        //emit animacijaZavrsila();
    }

    graph.push_back(Node(0, 0));
    int i = 1, j = 1;

    while (i < NW.size())
    {
        while (!_overlap[{NW[i].x(), NW[i].y()}] && i < NW.size())
            i++;
        while (!_overlap[{SE[j].x(), SE[j].y()}] && j < SE.size())
            j++;

        if (true)
        {
            Node newNode = Node(i-1, j-1);

            for (auto sState : nodeStates)
            {
                if (graph.size() > 1)
                {
                    bool hasIncomingArc = false;
                    for (auto state : nodeStates)
                    {
                        auto stateLinks = graph[graph.size()-2].links[state];
                        hasIncomingArc = hasIncomingArc || (std::find(stateLinks.begin(), stateLinks.end(), sState) != stateLinks.end());
                    }

                    if (!hasIncomingArc)
                        continue;
                }
                else
                {
                    if (sState != VH)
                        continue;
                }

                auto tState = checkIntersection(graph[graph.size()-1], newNode, sState, NW, SE);
                if (tState)
                {
                    graph[graph.size()-1].links[sState].push_back(tState.value());
                }

            }

            graph.push_back(newNode);
        }
        if (i < NW.size() && _overlap[{NW[i].x(), NW[i].y()}])
        {
            int i_old = i;

            while (_overlap[{NW[i].x(), NW[i].y()}] && i < NW.size())
                i++;
            while (_overlap[{SE[j].x(), SE[j].y()}] && j < SE.size())
                j++;

            Node newNode = Node(i, j);//Node(i-1, j-1);

            for (auto sState : nodeStates)
            {
                bool hasIncomingArc = false;
                for (auto state : nodeStates)
                {
                    auto stateLinks = graph[graph.size()-2].links[state];
                    hasIncomingArc = hasIncomingArc || (std::find(stateLinks.begin(), stateLinks.end(), sState) != stateLinks.end());
                }

                if (!hasIncomingArc)
                    continue;

                auto tStates = GOO(sState, i - i_old);
                for (auto tState : tStates)
                    graph[graph.size()-1].links[sState].push_back(tState);

            }
            graph.push_back(newNode);
        }
    }


    return graph;
}

std::pair<std::vector<QPoint>, std::vector<QPoint>> PolygonCover::separateRegion(int nw1, int se1, int nw2, NodeState incomingState, NodeState outgoingState, const std::vector<QPoint>& NW, const std::vector<QPoint>& SE)
{
    std::vector<QPoint> segmentNW, segmentSE;

    auto [oNW, oSE] = stateToOrientations(incomingState);

    for (int i = nw1 + 1, j = se1 + 1; i < nw2; i++, j++)
    {
        auto outgoingStates = GOO(orientationsToState(flipped(oNW),oSE), nw2 - i - 1);
        if (std::find(outgoingStates.begin(), outgoingStates.end(), outgoingState) != outgoingStates.end())
        {
            segmentNW.push_back(NW[i]);
            flip(oNW);
        }
        else
        {
            segmentSE.push_back(SE[j]); // SE[i] == NW[i]
            flip(oSE);
        }
    }

    return {segmentNW, segmentSE};
}

void PolygonCover::drawOverlaps()
{
    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();
    AlgoritamBaza_updateCanvasAndBlock();
}

std::pair<std::vector<QPoint>, std::vector<QPoint>> PolygonCover::createNWSE(const std::vector<QPoint>& points)
{


    auto [NW, SE] = separateNWSE(points);

    if (_infeasible)
       return {{},{}};


    drawOverlaps();

    SolutionGraph sg = createSolutionGraph(NW, SE);



    std::vector<QPoint> stairsNW, stairsSE;

    if (sg.size() > 1)
    {
        bool linksToHV = false;
        for (auto state : nodeStates)
        {
            auto stateLinks = sg[sg.size()-2].links[state];
            linksToHV = linksToHV || (std::find(stateLinks.begin(), stateLinks.end(), HV) != stateLinks.end());
        }
        if (!linksToHV)
        {
            qInfo() << "proper separation impossible!";
            _infeasible = true;
            return {{},{}};
            //emit animacijaZavrsila();
        }
    }

    bool overlapping = false;
    int i = sg.size() - 1;
    NodeState state = HV;
    while (i > 0) {
        if (overlapping)
        {
            auto outgoingState = state;
            state = getIncomingLinks(sg, i, state)[0];
            auto [segmentNW, segmentSE] = separateRegion(sg[i-1].i1, sg[i-1].i2, sg[i].i1, state, outgoingState, NW, SE);

            for (int j = segmentNW.size()-1; j >= 0; j--)
                stairsNW.push_back(segmentNW[j]);
            for (int j = segmentSE.size()-1; j >= 0; j--)
                stairsSE.push_back(segmentSE[j]);
        }
        else
        {
            for (int i1 = sg[i].i1; i1 >= sg[i-1].i1; i1--)
                stairsNW.push_back(NW[i1]);
            for (int i2 = sg[i].i2; i2 >= sg[i-1].i2; i2--)
                stairsSE.push_back(SE[i2]);

            state = getIncomingLinks(sg, i, state)[0];
        }
        overlapping = !overlapping;
        i--;
    }

    std::reverse(stairsNW.begin(), stairsNW.end());
    std::reverse(stairsSE.begin(), stairsSE.end());

    if (stairsNW.size() % 2 != 0 || stairsSE.size() % 2 != 0)
    {
        qInfo() << "failed to obtain proper separation!";
        _infeasible = true;
        return {{},{}};
    }

    return {stairsNW, stairsSE};
}


// input: vector of points in clockwise order starting from W
void PolygonCover::getFinalPolygon(const std::vector<QPoint>& points)
{

    int i = 1;
    while (i < points.size())
    {
        if (i % 2 == 0)
            _polygon.append(QPoint(points[i].x(), points[i-1].y()));
        else
            _polygon.append(QPoint(points[i-1].x(), points[i].y()));
        i++;
        AlgoritamBaza_updateCanvasAndBlock();
    }
    _polygon.append(QPoint(points[0].x(), points[points.size()-1].y()));

    AlgoritamBaza_updateCanvasAndBlock();
    _polygon.append(_polygon.front());

    AlgoritamBaza_updateCanvasAndBlock();
}




std::vector<NodeState> PolygonCover::GOO(NodeState sState, int pointCount)
{
    if (pointCount % 2 == 1)
    {
        switch (sState)
        {
            case HH: return {HV};
            case HV: return {};
            case VH: return {HH, VV};
            case VV: return {HV};
        }
    }
    else
    {
        switch (sState)
        {
            case HH: return {HH, VV};
            case HV: return {};
            case VH: return {HV};
            case VV: return {HH, VV};
        }
    }
}

std::pair<Orientation, Orientation> PolygonCover::stateToOrientations(NodeState state)
{
    switch (state)
    {
        case HH : return {H, H};
        case HV : return {H, V};
        case VH : return {V, H};
        case VV : return {V, V};
    }
}

NodeState PolygonCover::orientationsToState(Orientation o1, Orientation o2)
{
    if (o1 == H)
    {
        if (o2 == H)
            return HH;
        return HV;
    }
    if (o2 == H)
        return VH;
    return VV;
}

void PolygonCover::flip(Orientation& o)
{
    if (o == H)
        o = V;
    else
        o = H;
}

Orientation PolygonCover::flipped(Orientation o)
{
    flip(o);
    return o;
}

void PolygonCover::printPoints(std::vector<QPoint> points)
{
    for (auto& p : points)
        qInfo() << p.x() << "," << p.y() << " ";
}

bool PolygonCover::sortNW(QPoint a, QPoint b)
{
    if (a.x() == b.x())
        return a.y() < b.y();
    return a.x() < b.x();
}

bool PolygonCover::sortNE(QPoint a, QPoint b)
{
    if (a.x() == b.x())
        return a.y() > b.y();
    return a.x() < b.x();
}

bool PolygonCover::sortSE(QPoint a, QPoint b)
{
    if (a.x() == b.x())
        return a.y() > b.y();
    return a.x() > b.x();
}

bool PolygonCover::sortSW(QPoint a, QPoint b)
{
    if (a.x() == b.x())
        return a.y() < b.y();
    return a.x() > b.x();
}
