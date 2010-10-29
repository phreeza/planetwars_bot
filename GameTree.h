#ifndef GAMETREE_H
#define GAMETREE_H

#include "game.h"

#include <vector>


struct Order{
    int player_id, source, target, num_ships;

    Order(int _player_id,int _source, int _target, int _num_ships):
        player_id(_player_id),source(_source),target(_target),num_ships(_num_ships){}
};
typedef std::vector<Order> OrderVector;

struct UCTNode
{
    static const float logs[];
    GameState state; 
    UCTNode * parent;
    typedef std::vector<UCTNode *> UCTNodeVector;
    UCTNodeVector children;
    int n_total,sum_results,depth;
    float uct_val;
    OrderVector by_orders;

    UCTNode(GameState _state, UCTNode * _parent = NULL):
        state(_state),parent(_parent),children(UCTNodeVector()),n_total(0),sum_results(0),depth(0),uct_val(0.0) {}
    ~UCTNode();

    OrderVector RandomOrders();
    UCTNode * AddChild(OrderVector&, Game&);
    UCTNode * AddChild(GameState);
    void PruneOrders(Game & game);

    int FinalValue(Game&);
    void PropagateResult(int);
    float UCTValue(int N);
    UCTNode * FindMaxValue(int N);

};
#endif
