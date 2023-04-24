Sukesh Cheripalli, Puneet Udupi
Graduate team

# Assignment3_Demo



#### Solution Approach - Multi threading design

* There is a thread pool of num_threads which take "work" from a queue of available algo-house pairs. 
* Each thread has its own simulator which it reuses everytime to run the corresponding algorithm on the corresponding house. 
* Timeout for every thread is custom set to 10ms* maxsteps. 
* There is a single monitor thread that keeps checking if a thread has timedout or not, if it has timed out, it reduces the priority to idle, dumps the output and spawns another thread in its place.
* All the threads are detached from the main thread as we wait till running set and runnable queue is empty in main


#### Algorithm

The algorithm keeps track below parameters and use them to decide if the algorithm is completed.

- Perceived House - The points of the house which have been visited by the robot
- Unexplored Points - List of vertices which are known not walls and not visited by robot
- Total dirt - Total dirt as perceived by Algorithm at any point

Step by step logic is outlined below in the same order of priority:

* At each step neighbors are updated added to unexplored if not wall and not visited
* Unexplored points are given priority i.e. at any point if one of the neighbor is unexpored that is visited first
* If all neighbors are explored then robot visits location with maximum dirt
* Otherwise, `BFS` is performed on the percieved house to find the closest dirt or unexplored location and `move to position` is initiated
* At every step charging is compared with distance to dock on percieved house is calculated using `BFS` and if its in defined buffer threshold, `return to dock` is initiated. (TODO: Optimization by `distance map` to avoid BFS each time)
* Corner cases are handled seperately i.e. `max_battery` = 1 as DEAD case

Above logic is repeated until the `unexplored points is empty`, `total dirt is 0` and `robot and dock`.

#### Implementation

State machine is implemented by algorithm with `CHARGING`, `TO_DOCK`, `TO_POS`, `WORKING` states.

Initially, the algorithm will start in `WORKING` and current location is updated in percieved house and neighbors are updated in unexplored map of the house. Priority of next step is to clean the current tile completely i.e. `dirt_level = 0` then moving step is performed.

- If there are unexplored tiles in multiple directions, it uses a determinisitic priority list of directions.
- If all neighbors are explored, it picks the direction of tile which has the max dirt.
- Otherwise, the algorithm populates a stack with directions by performing `BFS` algorithm on the percieved house and changes the state to TO_POS.
- At every step including `Stay` step,
  - If we will have enough charge to go back to dock for charging and moved to `TO_DOCK` state
  - If the condition for `FINISH` has reached.
  - Once it reaches dock, it goes to `CHARGING` state until it reaches full charge and changes its state back to `WORKING`.
- In `TO_DOC`/TO_POS state, the algorithm just follows the stack of directions populated earlier by the bfs algorithm.
