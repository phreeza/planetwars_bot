#include <iostream>
#include <stdlib.h>
#include "GameTree.h"
#include "game.h"
#include "logs.h"

UCTNode::~UCTNode()
{
    for (UCTNodeVector::iterator it = children.begin();it!=children.end();it++)
        delete *it;
}

UCTNode * UCTNode::AddChild(GameState st)
{
    UCTNode * new_node = new UCTNode(st,this);
    children.push_back(new_node);
    return new_node;
}
void UCTNode::PruneOrders(Game & game)
{
    OrderVector tmp_orders = by_orders;
    int original_score = FinalValue(game);
    UCTNode * tmp_ptr;
    while((tmp_orders.size()==by_orders.size())&&(tmp_orders.size()>0))
    {
        //for (OrderVector::iterator it = by_orders.begin();it!=by_orders.end();it++)
        for (int it = 0;it<(int)by_orders.size();it++)
        {
            tmp_orders = by_orders;
            tmp_orders.erase(tmp_orders.begin()+it);
            tmp_ptr = parent->AddChild(tmp_orders,game);
            if(tmp_ptr->FinalValue(game)>=original_score)
            {
                by_orders = tmp_orders;
                break;
            }
        } 
    }
}

OrderVector UCTNode::SemiRandomOrders()
{
	typedef std::vector<PlanetState> Planets;
    float p_send = 0.3;
    OrderVector ret;
    int player = (depth % 2) + 1;
    for(int p = 0;(size_t)p<state.planets.size();p++)
    {
        if ((state.planets[p].owner == player)
                &&(state.planets[p].numShips > 0)
                &&((float)rand()/RAND_MAX<p_send))
        {
            int target = p;
            int tosend = 0;
            while (target == p)
            {
                target = rand()%state.planets.size();
            }
            if (state.planets[target].owner != player)
            {
                switch(rand()%2)
                {
                /*case 0:
                    //send a random ammount
                    tosend = (rand()%(state.planets[p].numShips))+1;
                    break;*/
                case 0:
                    //send just what is needed to conquer
                    tosend = std::min(state.planets[p].numShips-5,state.planets[target].numShips+1);
                    break;
                case 1:
                    //send all out attack
                    tosend = state.planets[p].numShips-5;
                    break;
                }
            }
            else
            {
                switch(rand()%2)
                {
                case 0:
                    //send stream
                    tosend = std::min(state.planets[p].numShips-5,5);
                    break;
                case 1:
                    //send all out defense
                    tosend = state.planets[p].numShips-5;
                    break;
                }
            }
            if (tosend>0)
                ret.push_back(
                    Order(player,
                        p,
                        target,
                        tosend)
                    );
        }
    }
   return ret; 
}

OrderVector UCTNode::RandomOrders()
{
	typedef std::vector<PlanetState> Planets;
    float p_send = 0.3;
    OrderVector ret;
    int player = (depth % 2) + 1;
    for(int p = 0;(size_t)p<state.planets.size();p++)
    {
        if ((state.planets[p].owner == player)
                &&(state.planets[p].numShips > 0)
                &&((float)rand()/RAND_MAX<p_send))
        {
            int target = p;
            while (target == p)
            {
                target = rand()%state.planets.size();
            } 
            ret.push_back(
                    Order(player,
                        p,
                        target,
                        (rand()%(state.planets[p].numShips))+1)
                    );
        }
    }
   return ret; 
}

UCTNode * UCTNode::AddChild(OrderVector & orders, Game & game)
{
    game.state = state;
    for (OrderVector::iterator o = orders.begin();o!=orders.end();o++)
    {
        game.state.ExecuteOrder(game.desc,o->player_id,o->source,o->target,o->num_ships); 
    }
    if(depth%2==1)
        game.DoTimeStep();
    UCTNode * ret = new UCTNode (game.state,this);
    ret->depth = depth+1;
    ret->time_step = time_step+1;
    if (ret->depth == 1)
        ret->by_orders = orders;
    children.push_back(ret);
    return ret;
}

int UCTNode::FinalValue(Game & game)
{
    game.state = state;
    while((game.state.fleets.size()>0)&&(time_step<200))
    {
        game.DoTimeStep();
        time_step++;
    }
    int n1 = state.NumShips(1); 
    int n2 = state.NumShips(2); 
    if (n1==0) return -99999;
    else if (n2==0) return 99999;
    else return (n1-n2) + (200-time_step)*(game.Production(1)-game.Production(2));
}
void UCTNode::PropagateResult(float result)
{
    n_total++;
    sum_results += result;
    if (parent != NULL)
        parent->PropagateResult(-result);
}

float InvSqrt(float x)
{
    union {
        float f;
        int i;
    } tmp;
    tmp.f = x;
    tmp.i = 0x5f3759df - (tmp.i >> 1);
    float y = tmp.f;
    return y * (1.5f - 0.5f * x * y * y);
}

float UCTNode::UCTValue(int N)
{
    float avg = ((float)sum_results)/(n_total);
    float ln;
    if (N<10000)
        ln = logs[N];
    else
        ln = log(N);
    //float sigm = avg/sqrt(1+(avg*avg));
    return avg+1.1*sqrt(ln/(n_total));
    //return avg+1.4*InvSqrt(((float)n_total)/ln);
}

UCTNode * UCTNode::FindMaxValue(int N)
{
    N = 0;
    float max,tmp_val;
    UCTNode * tmp;
    UCTNode * ret;
    if (children.size() == 0) return this;
    

    for (UCTNodeVector::iterator it = children.begin();it!=children.end();it++)
    {
        N += (*it)->n_total;
    }
    ret = children[0];
    max = children[0]->UCTValue(N);
    for (UCTNodeVector::iterator it = children.begin();it!=children.end();it++)
    {
        tmp = *it;
        tmp_val = tmp->UCTValue(N);
        if (tmp->n_total == 0)
        {
            ret = tmp;
            break;
        }
        if (tmp_val>max)
        {
            max = tmp_val;
            ret = tmp;
        }
    }
    return ret->FindMaxValue(N);
}
