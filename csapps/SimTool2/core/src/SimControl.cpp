/****************************************************************************
** Copyright (c) 2012,2016, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "SimCore/SimControl.h"

#include "SimCore/SimContext.h"

////// public ////////////////////////////////////////////////////////////////

SimControl::SimControl(SimContext *simctx)
  : QObject(simctx)
  , _stateMachine(0)
  , _idleState(0)
  , _initState(0)
  , _pauseState(0)
  , _startState(0)
  , _stepState(0)
  , _stopState(0)
{
  // (1) Create Objects //////////////////////////////////////////////////////

  _stateMachine = new QStateMachine(this);
  _idleState  = new QState();
  _initState  = new QState();
  _pauseState = new QState();
  _startState = new QState();
  _stepState  = new QState();
  _stopState  = new QState();

  // (2) Create Transitions //////////////////////////////////////////////////

  _idleState->addTransition(this, &SimControl::startRequested, _initState);

  _initState->addTransition(_startState);

  _startState->addTransition(_stepState);

  _stepState->addTransition(this, &SimControl::stopRequested, _stopState);
  _stepState->addTransition(this, &SimControl::pauseRequested, _pauseState);

  _pauseState->addTransition(this, &SimControl::startRequested, _stepState);
  _pauseState->addTransition(this, &SimControl::stopRequested, _stopState);

  _stopState->addTransition(_idleState);

  // (3) Initialize State Machine ////////////////////////////////////////////

  _stateMachine->addState(_idleState);
  _stateMachine->addState(_initState);
  _stateMachine->addState(_pauseState);
  _stateMachine->addState(_startState);
  _stateMachine->addState(_stepState);
  _stateMachine->addState(_stopState);
  _stateMachine->setInitialState(_idleState);

  // (4) Signals & Slots /////////////////////////////////////////////////////

  connect(_idleState, &QState::entered, this, &SimControl::enterState);
  connect(_initState, &QState::entered, this, &SimControl::enterState);
  connect(_pauseState, &QState::entered, this, &SimControl::enterState);
  connect(_startState, &QState::entered, this, &SimControl::enterState);
  connect(_stepState, &QState::entered, this, &SimControl::enterState);
  connect(_stopState, &QState::entered, this, &SimControl::enterState);

  connect(_idleState, &QState::exited, this, &SimControl::exitState);
  connect(_initState, &QState::exited, this, &SimControl::exitState);
  connect(_pauseState, &QState::exited, this, &SimControl::exitState);
  connect(_startState, &QState::exited, this, &SimControl::exitState);
  connect(_stepState, &QState::exited, this, &SimControl::exitState);
  connect(_stopState, &QState::exited, this, &SimControl::exitState);

  // (5) Go! /////////////////////////////////////////////////////////////////

  _stateMachine->start();
}

SimControl::~SimControl()
{
}

////// private slots /////////////////////////////////////////////////////////

void SimControl::enterState()
{
  if(        sender() == _idleState ) {
    emit stateEntered(IdleState);
  } else if( sender() == _initState ) {
    emit stateEntered(InitState);
  } else if( sender() == _pauseState ) {
    emit stateEntered(PauseState);
  } else if( sender() == _startState ) {
    emit stateEntered(StartState);
  } else if( sender() == _stepState ) {
    emit stateEntered(StepState);
  } else if( sender() == _stopState ) {
    emit stateEntered(StopState);
  } else {
    emit stateEntered(InvalidOperationState);
  }
}

void SimControl::exitState()
{
  if(        sender() == _idleState ) {
    emit stateExited(IdleState);
  } else if( sender() == _initState ) {
    emit stateExited(InitState);
  } else if( sender() == _pauseState ) {
    emit stateExited(PauseState);
  } else if( sender() == _startState ) {
    emit stateExited(StartState);
  } else if( sender() == _stepState ) {
    emit stateExited(StepState);
  } else if( sender() == _stopState ) {
    emit stateExited(StopState);
  } else {
    emit stateExited(InvalidOperationState);
  }
}
