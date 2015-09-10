Project: AISweeper
Author: Alex Johnson
Description: A modular Minesweeper platform that allows for experimentation with different AIs.
Notes:

9/9/2015
========
Documentation is hard, let's just write code!

8/10/2015
=========
Additional thoughts:
•Can remove flagging from Controller/Model/Logic and leave that to the Input/View classes.  It has no effect on the actual game logic.  The win condition in Minesweeper is to have all of the non-mined tiles revealed.  The remaining tiles can be either flagged or not.  The flags are merely there to help the user keep track of where they think the mines are.  This could be handled by the AI itself, or tacked on to the View as something that it keeps track of separately.  In this case the event handlers for flagging would redirect to classes in the Input/View instead of letting the Controller/Model/Logic know that the click has taken place.
•Likewise, can remove the SweeperNodeState class and simply maintain two bools: mined + revealed.  The numbers are a visual element that can be calculated by the view itself.  Unfortunately, the view will run less efficiently this way since it would need to re-calculate the numbers on each update cycle instead of relying on them statically in the model.  It could attempt to cache the numbers itself, but then we'd need a way to let the view know when to recalculate them.  Directly messaging the view would be a violation of the 'view as an observer' principle.  The view could, in theory, run a hash of the entire model to see if anything has changed between render cycles and only recalculate the nearby mine numbers if the model had been updated.  I believe hashing the model would be just as computationally expensive as simply re-calculating the nearby mine counts on each cycle.  All things considered, I believe recalculation is acceptable.  When running headlessly in parallel in conjunction with AI this will be a non-issue since there will be no view.  Since this is the only instance where performance truly matters the game should be optimized to run as lightly as possible.  Offloading calculation of the nearby mine count to the View makes sense in this case.
•Input will need a way to access the View and visa-versa.  This is so the player can make Input decisions based on the View and so that the View can render the Input itself as the user makes decisions (IE: show pressed version of each node before left-click is released and Input is actually sent to the Controller).  This also helps to synchronize the input/output in the console versions of View/Input so that refresh cycles don't overwrite the input prompts.  The current unsubmitted input can be saved externally and re-rendered on each cycle.
•I'll start with the console Input/View before learning QT.  I believe the SweeperView class will need a pure virtual initialize function.  This is where QT can spin up its own thread(s).  However, for the console we'll call the View directly at the beginning of each getNextInput() call from the Controller.  We'll do this by using some sort of renderLastInput() function.

8/9/2015
========
Initial UML diagram (definitely subject to change):
.--------------------------------------------------------------.
|                                                              |
|   .-----------------------------------.        .---------------------------.
| .-|           SweeperModel            |-----.  |   SweeperView *thread2*   |
| | .-----------------------------------.     |  .---------------------------.
| | | gameState : SweeperGameState      |<--. '->| model : SweeperModel      |
| | | nodeStates : [][]SweeperNodeState |<. |    | config : SweeperConfig    |<.
| | | minedStates : [][]bool            | | |    .---------------------------. |
| | '-----------------------------------' | |    | SweeperView(model/config) | |
| | .-----------------------------.       | |    '---------------------------' |
| | | SweeperNodeState << Enum >> |-------' |    .-----------. ^ .-----------. |
| | .-----------------------------.         |    |  Console  |-'-|  OpenGL   | |
| | | Hidden                      |         |    '-----------' | '-----------' |
| | | Flagged                     |         |    .-----------. | .-----------. |
| | | Empty                       |         |    |    QT     |-'-| Headless  | |
| | | ByOne                       |         |    '-----------'   '-----------' |
| | | ByTwo                       | .----------------------------------------. |
| | | ByThree                     | |      SweeperGameState << Enum >>       | |
| | | ByFour                      | .----------------------------------------. |
| | | ByFive                      | | Loading                                | |
| | | BySix                       | | Playing                                | |
| | | BySeven                     | | Lost                                   | |
| | | ByEight                     | | Won                                    | |
| | | Detonated                   | | Exiting                                | |
| | '-----------------------------' '----------------------------------------' |
| |                                                                            |
| '-------------------------------------------------------.                    |
| |                                                       v                    |
| | .-----------------------------.   .--------------------------------------. |
| | | SweeperController *thread1* | .-|             SweeperLogic             | |
| | .-----------------------------. | .--------------------------------------. |
| '>| model : SweeperModel        | | | reveal(model/row/col) *static*       | |
'-->| view : SweeperView          | | | flag(model/row/col) *static*         | |
    | logic : SweeperLogic        |<' | populateModel(model/config) *static* |<.
    | input: SweeperInput         |<. '--------------------------------------' |
  .>| config : SweeperConfig      | |  .-----------------------------------.   |
  | .-----------------------------. '--|           SweeperInput            |   |
  | | main(args) *static*         |    .-----------------------------------.   |
  | '-----------------------------'  .>| config : SweeperConfig            |   |
  | .------------------------------. | .-----------------------------------.   |
  '-|        SweeperConfig         |-' | getNextInput() : command/row/col  |<. |
    .------------------------------. | '-----------------------------------' | |
.-->| viewType : SweeperViewType   | | .--------------. ^ .----------------. | |
| .>| inputType : SweeperInputType | | |   Console    |-'-|   CompiledAI   | | |
| | | height : int                 | | '--------------' | '----------------' | |
| | | width : int                  | | .--------------. | .----------------. | |
| | | mineCount : int              | | |      QT      |-'-| SocketListener | | |
| | | randomSeed : int             | | '--------------'   '----------------' | |
| | .------------------------------. | .-----------------------------------. | |
| | | SweeperConfig(args)          | | |   SweeperCommandType << Enum >>   |-' |
| | '------------------------------' | .-----------------------------------.   |
| | .------------------------------. | | Reveal                            |   |
| '-| SweeperInputType << Enum >>  | | | Flag                              |   |
|   .------------------------------. | | Exit                              |   |
|   | Console                      | | '-----------------------------------'   |
|   | QT                           | '-----------------------------------------'
|   | CompiledAI                   |   .------------------------------.
|   | SocketListener               | .-|  SweeperViewType << Enum >>  |
|   '------------------------------' | .------------------------------.
|                                    | | Console                      |
'------------------------------------' | QT                           |
                                       | OpenGL                       |
                                       | Headless                     |
                                       '------------------------------'
