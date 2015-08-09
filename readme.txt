Project: AISweeper
Author: Alex Johnson
Description: A modular Minesweeper platform that allows for experimentation with different AIs.
Notes:

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
