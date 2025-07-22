#include "fsm.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

// Definisi millis() sesuai header
uint32_t millis() {
    static auto start_time = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count();
}

// Konstruktor default
FSM::FSM() : currentState(SystemState::INIT), lastHeartbeat(0), delay(0), errorCount(0), moveCount(0) {
    stateHistory.clear();
    stateHistory.emplace_back(currentState, lastHeartbeat);
}

// Konstruktor dengan delay
FSM::FSM(uint32_t delay_ms) : currentState(SystemState::INIT), lastHeartbeat(0), delay(delay_ms), errorCount(0), moveCount(0) {
    stateHistory.clear();
    stateHistory.emplace_back(currentState, lastHeartbeat);
}

// Destruktor
FSM::~FSM() {
    stateHistory.clear();
}

// Getter current state
SystemState FSM::getCurrentState() const {
    return currentState;
}

// Transisi ke state baru
void FSM::transitionToState(SystemState newState) {
    currentState = newState;
    lastHeartbeat = millis();
    stateHistory.emplace_back(newState, lastHeartbeat);
}

void FSM::setDelay(uint32_t d) { delay = d; }
void FSM::getDelay(uint32_t &d) const { d = delay; }
void FSM::setErrorCount(int count) { errorCount = count; }
int FSM::getErrorCount() const { return errorCount; }
void FSM::setMoveCount(int count) { moveCount = count; }
int FSM::getMoveCount() const { return moveCount; }

void FSM::addStateToHistory(SystemState state, uint32_t time) {
    stateHistory.emplace_back(state, time);
}
vector<pair<SystemState, uint32_t>> FSM::getStateHistory() const {
    return stateHistory;
}
uint32_t FSM::getLastHeartbeat() const { return lastHeartbeat; }
void FSM::setLastHeartbeat(uint32_t heartbeat) { lastHeartbeat = heartbeat; }

// Start FSM: inisialisasi lalu loop hingga STOPPED
void FSM::start() {
    performInit();
    while (currentState != SystemState::STOPPED) {
        update();
    }
    shutdown();
}

// Update sesuai state saat ini
void FSM::update() {
    switch (currentState) {
        case SystemState::INIT:        performInit();         break;
        case SystemState::IDLE:        performProcess();      break;
        case SystemState::MOVEMENT:    performMovement();     break;
        case SystemState::SHOOTING:    performShooting();     break;
        case SystemState::CALCULATION: performCalculation();  break;
        case SystemState::ERROR:       performErrorHandling();break;
        case SystemState::STOPPED:     shutdown();            break;
    }
}

// Cetak status ringkas
void FSM::printStatus() {
    cout << "[Status] State=" << static_cast<int>(currentState)
         << " MoveCount=" << moveCount
         << " Errors=" << errorCount << endl;
}

// Cetak histori state
void FSM::printStateHistory() {
    cout << "[History]";
    for (auto &entry : stateHistory) {
        cout << " (" << static_cast<int>(entry.first) << "," << entry.second << ")";
    }
    cout << endl;
}

// Inisialisasi
void FSM::performInit() {
    cout << "Initializing..." << endl;
    transitionToState(SystemState::IDLE);
}

// Tunggu input user
void FSM::performProcess() {
    printStatus();
    cout << "Commands: 1=Status 2=Move 3=Shoot 4=Calc 5=Stop > ";
    int cmd; cin >> cmd;
    switch (cmd) {
        case 1: printStatus(); printStateHistory(); break;
        case 2: transitionToState(SystemState::MOVEMENT); break;
        case 3: transitionToState(SystemState::SHOOTING); break;
        case 4: transitionToState(SystemState::CALCULATION); break;
        case 5: transitionToState(SystemState::STOPPED); break;
        default: cout<<"Invalid"<<endl; transitionToState(SystemState::ERROR);
    }
}

// Proses Movement
void FSM::performMovement() {
    cout << "Moving..." << endl;
    moveCount++;
    transitionToState(moveCount>=3 ? SystemState::SHOOTING : SystemState::IDLE);
}

// Proses Shooting
void FSM::performShooting() {
    cout << "Shooting..." << endl;
    moveCount = 0;
    transitionToState(SystemState::IDLE);
}

// Proses Calculation
void FSM::performCalculation() {
    cout << "Calculating..." << endl;
    if (moveCount==0) transitionToState(SystemState::ERROR);
    else               transitionToState(SystemState::IDLE);
}

// Handle Error
void FSM::performErrorHandling() {
    cout << "Error!" << endl;
    errorCount++;
    transitionToState(errorCount>3 ? SystemState::STOPPED : SystemState::IDLE);
}

// Shutdown
void FSM::shutdown() {
    cout << "Shutting down..." << endl;
    printStateHistory();
}
