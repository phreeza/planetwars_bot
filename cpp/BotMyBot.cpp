#include <iostream>
#include <time.h>
#include "game.h"
#include "GameTree.h"
#include "utils.h"

void DoTurn(Game & pw) {

    GameState gs(pw.state);
    UCTNode root(gs);
    root.time_step = pw.numTurns;
    float tmp_val;
    UCTNode * tmp_ptr;
    long endwait;
    endwait = currentTimeMillis() + 900;
    int i=0;
   /* 
    for (int n = 0;n<1000;n++)
    {
        OrderVector o = root.RandomOrders(); 
        tmp_ptr = root.AddChild(o,pw);
        tmp_val = tmp_ptr->FinalValue(pw);
        tmp_ptr->PropagateResult(tmp_val);
        i++;

    }*/
    int tree_width[3] = {500,100,5};
    int n_nodes = 0;
    int baseline_val = root.FinalValue(pw);
    OrderVector o = root.SemiRandomOrders();
    root.AddChild(o,pw);
    while(currentTimeMillis()<endwait)
    {
        tmp_ptr = root.FindMaxValue(i+1);
        if ((int)tmp_ptr->parent->children.size()<tree_width[std::min(2,tmp_ptr->depth-1)])
        //if(rand()%2==0)
        {
            OrderVector o = tmp_ptr->parent->RandomOrders();
            tmp_ptr = tmp_ptr->parent->AddChild(o,pw);
            n_nodes++;
        }
       
        if (baseline_val != 0) 
            tmp_val = ((float)(tmp_ptr->FinalValue(pw)-baseline_val))/abs(baseline_val);
        else
            tmp_val = ((float)(tmp_ptr->FinalValue(pw)-baseline_val));
        if(tmp_ptr->depth%2==0)
            tmp_ptr->PropagateResult(-tmp_val);
        else
            tmp_ptr->PropagateResult(tmp_val);
        OrderVector o = tmp_ptr->RandomOrders();
        tmp_ptr->AddChild(o,pw);
        n_nodes++;
        i++;
    }
    UCTNode * dothis = root.children[0];
    int maxplayed = -9999;
    for (UCTNode::UCTNodeVector::iterator it = root.children.begin();it!=root.children.end();it++)
    {
        //std::cerr << (*it)->n_total << std::endl;
        int tmp_score = (*it)->n_total;
        if (tmp_score>maxplayed)
        {
            maxplayed = tmp_score;
            dothis = *it;
        }
    }
    //dothis->PruneOrders(pw);
    std::cerr << i <<"\t" <<root.n_total <<"\t"<< dothis->n_total<< "\t" <<root.children.size() <<"\t" << n_nodes<< "\t"<<((float)dothis->sum_results/dothis->n_total) <<"\t" <<baseline_val <<std::endl;


    for (OrderVector::iterator o = dothis->by_orders.begin();o!=dothis->by_orders.end();o++)
    {
        if (o->player_id==1)
            pw.IssueOrder(o->source,o->target,o->num_ships); 
    }
}

int PlayGame(void* p = NULL) {
    bool isFirstTurn = true;
    std::string current_line;
    std::string map_data;
    while (true) {
        int c = std::cin.get();
        if(c < 0) break;
        current_line += (char)(unsigned char)c;
        if (c == '\n') {
            if(current_line.substr(0, 3) == "end")
                break;
            if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
                Game game;
                game.ParseGameState(map_data);
                map_data = "";
                DoTurn(game);
                game.FinishTurn();
                isFirstTurn = false;
            } else {
                map_data += current_line;
            }
            current_line = "";
        }
    }
    return 0;
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
    PlayGame();
    return 0;
}
