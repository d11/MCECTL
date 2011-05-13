#!/usr/bin/env python2
# 
# This script generates an ECTL input file for the 'missionaries and cannibals'
# problem
#

n = 4
states = [ (i,j) for i in range(n) for j in range(n) ]

def state_name(i,j):
   return "h" + str(i) + "_l" + str(j)

print "LTS river {"

for (i,j) in states:
   propositions = [];
   if i != j:
      propositions.append("carnage")
   if i == 0 and j == 0:
      propositions.append("success")
   print "   STATE ( " + state_name(i,j) + ": " + ",".join(propositions) + " )"

def transition(i,j,k,l,action):
   if k >= 0 and k < n and l >= 0 and l < n:
      print "   ACTION ( " + action + ": " + state_name(i,j) + " -> " + state_name(k,l) + " )"

for (i,j) in states:
   transition(i,j,i-1,j,"h1_across")
   transition(i,j,i-2,j,"h2_across")
   transition(i,j,i-1,j-1,"h1l1_across")
   transition(i,j,i,j-1,"l1_across")
   transition(i,j,i,j-2,"l2_across")
   transition(i,j,i+1,j,"h1_back")
   transition(i,j,i+2,j,"h2_back")
   transition(i,j,i+1,j+1,"h1l1_back")
   transition(i,j,i,j+1,"l1_back")
   transition(i,j,i,j+2,"l2_back")


print "}"

print "PDA boat {"
print "   STATE ( here )"
print "   STATE ( *there )"
for s in ["h1","h2","h1l1","l1","l2"]:
   print "   ACTION ( " + s + "_across: here[_] -> there[REWRITE _] )"
   print "   ACTION ( " + s + "_back: there[_] -> here[REWRITE _] )"
print "}"

print "FORMULA solve {"
print "   E( !carnage U[boat] success )"
print "}"
print ":check(solve,river)"
