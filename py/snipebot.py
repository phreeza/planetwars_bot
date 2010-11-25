#!/usr/bin/python

from planetwars import BaseBot, Game
import random
from planetwars.universe import Universe

from logging import getLogger

log = getLogger(__name__)


class SnipeBot(BaseBot):
    def __init__(self, universe):
        self.universe = universe
        self.turn = 0

    def do_turn(self):
        self.turn += 1
        if self.turn == 1:
            self.do_greedy()
            return
        future = Future(self.universe,self.turn)
        for l in future.owner:
            log.debug(l)
        sent = {}
        for p in self.universe.my_planets:
            sent[p] = 0
            for e in self.universe.planets:
                if p == e:
                    continue
                reach_turn = self.turn + p.distance(e)
                if reach_turn >= 200:
                    continue
                if(future.owner[199][e.id] == 2 and future.owner[reach_turn][e.id] == 2 and future.ship_count[reach_turn][e.id]<p.ship_count-5-sent[p]):
                    p.send_fleet(e,future.ship_count[reach_turn][e.id]+5)
                    sent[p] += future.ship_count[reach_turn][e.id]+5
    def do_greedy():
        for p in self.universe.my_planets:
            min_dist = 9999
            min_planet = None
            for e in self.universe.nobodys_planets:
                if e.distance(p) < min_dist:
                    min_planet = e
                    min_dist = e.distance(p)
            p.send_fleet(min_planet,min_planet.ship_count+5)

        
class Future:
    def __init__(self,universe,turn):
        self.my_arrivals = [[0 for r in universe.planets] for q in range(200)]
        self.enemy_arrivals = [[0 for r in universe.planets] for q in range(200)]
        self.owner = [[0 for r in universe.planets] for q in range(200)]
        self.ship_count = [[0 for r in universe.planets] for q in range(200)]
       
        for r in universe.planets:
            for i in range(200):
                self.owner[i][r.id] = r.owner.id 
                self.ship_count[i][r.id] = r.ship_count 
        for f in universe.my_fleets:
            if turn+f.turns_remaining >= 200:
                continue
            self.my_arrivals[turn+f.turns_remaining][f.destination.id] += f.ship_count
        for f in universe.enemy_fleets:
            if turn+f.turns_remaining >= 200:
                continue
            self.enemy_arrivals[turn+f.turns_remaining][f.destination.id] += f.ship_count
        for t in range(turn+1,200):
            for p in universe.planets:
                self.owner[t][p.id] = self.owner[t-1][p.id]
                self.ship_count[t][p.id] = self.ship_count[t-1][p.id]
                if self.owner[t][p.id] > 0:
                    self.ship_count[t][p.id] += p.growth_rate
                self.ship_count[t][p.id], self.owner[t][p.id] = self._resolve(
                        [0, self.my_arrivals[t][p.id], self.enemy_arrivals[t][p.id]]
                        , self.ship_count[t][p.id], self.owner[t][p.id]
                        )

    def _resolve(self,fleets,planet_fleets,owner):
        fleets[owner] += planet_fleets
        startfleets = fleets[owner]
        top = max(fleets)
        top_ind = fleets.index(top)
        fleets.remove(top)
        sec = max(fleets)

        if (sec==top):
            return (owner,startfleets-top)
        else:
            return (top-sec,top_ind)



class MyUniverse(Universe):
    @property
    def my_and_nobodies_planets(self):
        return self.my_planets | self.nobodies_planets

Game(SnipeBot, universe_class=MyUniverse)
