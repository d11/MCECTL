#!/usr/bin/env python2
# 
# This script generates an ECTL input file for the 'missionaries and cannibals'
# problem
#

n = 4
states = [ (i,j,a) for i in range(n) for j in range(n) for a in range(2) ]

def state_name(i,j,a):
   return "h" + str(i) + "_l" + str(j) + "_" + str(a)

for (i,j,a) in states:
   propositions = [];
   if (a == 0 and i < j) or (a == 1 and j < i):
      propositions.append("carnage")
   if i == 0 and j == 0:
      propositions.append("success")
   print "   STATE ( " + state_name(i,j,a) + ": " + ",".join(propositions) + " )"

def transition((i,j,a),(k,l,b),action):
   if k >= 0 and k < n and l >= 0 and l < n:
      print "   ACTION ( " + action + ": " + state_name(i,j,a) + " -> " + state_name(k,l,b) + " )"

for (i,j,a) in states:
   transition((i,j,a),(i,j,1-a),"across")
