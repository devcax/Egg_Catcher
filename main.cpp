#include "gameEngine.h"

using namespace gameEngine;

/**
 * Check if all the frame files exist in the given folder.
 * @param folder The folder containing the frame files.
 * @return True if all the frame files exist, false otherwise.
 */
bool CheckFramesFilesExist(std::string& folder) {
    for (int i = 0; i <= 9; ++i) {
        std::string filename = folder + "/" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (!file.good()) {
            return false;
        }
    }
    return true;
}

/**
 * Run the game.
 */
int main() {
    std::string framesFolder = "frames";
    if (!CheckFramesFilesExist(framesFolder)) {
        MessageBox(NULL, "One or more frame files do not exist in the path.", "Warning", MB_ICONWARNING | MB_OK);
        return 1;
    }
    RunGame();
    return 0;
}