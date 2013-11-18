# 4thYear

Model Checker for Extended Computation Tree Logic


Abstract
---
Computation Tree Logic is widely used for modelling the behaviour of simple systems over time, but its expressive power is limited. More powerful logics such as CTL* and the modal mu-calculus have the disadvantage that their model-checking problems are comparatively intractable, and they can be unintuitive. A recent contribution by Axelsson et al. introduces the 'Extended CTL' family of logics, in which the Until and Release operators of CTL are parameterised by various classes of automaton. This has the advantage of increasing the expressive power of the logic, whilst (in the case of pushdown automata) preserving the tractability of model checking. Using algorithms based on those described in the paper, this project implements (to the best of my knowledge) the first concrete system for model checking CTL[PDA, DPDA], the logic in which both operators are refined by pushdown automata, which are deterministic in the case of Release. As well as a robust and tested core checking procedure, we provide a set of commands for loading and displaying automata, systems and formulas. It is possible to check both regular and pushdown systems, and checking a fixed formula is possible in time quadratic in the size of the system and sizes of the automata used.


See [http://www.dhorgan.com/things/mcectl/](this page) for more information.
