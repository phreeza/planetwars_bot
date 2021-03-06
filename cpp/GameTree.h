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
    int n_total,depth,time_step;
    float uct_val,sum_results;
    OrderVector by_orders;

    UCTNode(GameState _state, UCTNode * _parent = NULL):
        state(_state),parent(_parent),children(UCTNodeVector()),n_total(0),depth(0),time_step(0),uct_val(0.0),sum_results(0.0) {}
    ~UCTNode();

    OrderVector RandomOrders();
    OrderVector SemiRandomOrders();
    UCTNode * AddChild(OrderVector&, Game&);
    UCTNode * AddChild(GameState);
    void PruneOrders(Game & game);

    int FinalValue(Game&);
    void PropagateResult(float);
    float UCTValue(int N);
    UCTNode * FindMaxValue(int N);

};
#endif
