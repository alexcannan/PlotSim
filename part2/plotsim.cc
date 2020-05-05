#include <thread>

#include "app.hh"
#include "timer.hh"
#include "mysimulator.hh"

static void userInit(brown::Simulator& sim) {
    // your amazing code
    sim->timx.setPrescaler(1000);
    sim->timx.setPeriod(120);
    printf("In userInit\n");
}

static void userLoop(brown::Simulator& sim) {
    // your amazing code
    printf("In userLoop\n");
}

static void timxCallback(brown::Timer& tim, brown::Simulator& sim) {
    // your amazing code
    printf("In timxCallback\n");
}

static void timyCallback(brown::Timer& tim, brown::Simulator& sim) {
    // your amazing code
    printf("In timyCallback\n");
}

int main(int argc, const char * argv[]) {
    brown::Timer timx(timxCallback);
    brown::Timer timy(timyCallback);
    brown::MySimulator sim(timx, timy);
    brown::App app(sim, userInit, userLoop);
    std::thread logicThread(&brown::App::runLogic, &app);
    int retCode = app.runGraphics();
    logicThread.join();
    return retCode;
}
