#!/usr/bin/python

from planetwars import BaseBot, Game
import random
from planetwars.universe import Universe

# This shows how you can add your own functionality to game objects (Universe in this case).

class StupidBot(BaseBot):
    def __init__(self, universe):
        self.universe = universe
        self.turn = 0

    def do_turn(self):
        self.turn += 1
        future = Future(self.universe,self.turn)
        for p in self.universe.my_planets:
            for e in self.universe.planets:
                reach_turn = self.turn + p.distance(e)
                if(future.owner[reach_turn][e.id] == 2 and future.ship_count[reach_turn][e.id]<p.ship_count-5)
                    p.send_fleet(future.ship_count[reach_turn][e.id]+5)

class Future:
    def __init__(self,universe,turn):
        self.my_arrivals = [[0 for r in universe.planets] for q in range(200)]
        self.enemy_arrivals = [[0 for r in universe.planets] for q in range(200)]
        self.owner = [[r.owner for r in universe.planets] for q in range(200)]
        self.ship_count = [[r.ship_count for r in universe.planets] for q in range(200)]
        for f in universe.my_fleets:
            mself.y_arrivals[turn+f.turns_remaining][f.destination] += f.ship_count
        for f in universe.enemy_fleets:
            eself.nemy_arrivals[turn+f.turns_remaining][f.destination] += f.ship_count
        for t in range(turn+1,200):
            for p in universe.planets:
                self.owner[t][p.id] = self.owner[t-1][p.id]
                self.ship_count[t][p.id] = self.ship_count[t-1][p.id]
                if self.owner[t][p.id] > 0:
                    self.ship_count[t][p.id] += p.growth_rate
                self.ship_count[t][p.id], self.owner[t][p.id] = resolve(
                        [0, self.my_arrivals[t][p.id], self.enemy_arrivals[t][p.id]]
                        , self.ship_count[t][p.id], self.owner[t][p.id]
                        )

    def _resolve(self,fleets,planet_fleets,owner):
        fleets[owner] += planet_fleets
        top = max(fleets)
        top_ind = fleets.index(top)
        fleets.remove(top)

        old_index = [[1,0, 0],[2, 2, 1]]
        sec = max(fleets)
        sec_ind = old_index[fleets.index(sec)][old_index]

        if (sec==top):
            return (owner,max(0,fleets[owner]-top)
        else:
            return (top_ind,top-sec)



class MyUniverse(Universe):
    @property
    def my_and_nobodies_planets(self):
        return self.my_planets | self.nobodies_planets

Game(StupidBot, universe_class=MyUniverse)
