/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "network/network-manager.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "protocol-state-machine.h"

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Creates new ProtocolStateMachine associated with given protocol handler.
 * @param protocol protocol handler to connect this object to
 * @param parent QObject parent of this object
 *
 * This contructor creates new ProtocolStateMachine and connects to given protocol handler. All states
 * are set up. Depending on current network state (see NetworkManager::isOnline()) starting state
 * is selected: either logged out, online or logged out, offline. State machine is then started.
 */
ProtocolStateMachine::ProtocolStateMachine(Protocol *protocol, QObject *parent) :
		QStateMachine(parent), CurrentProtocol(protocol)
{
	LoggingOutState = new QState(this);
	LoggedOutOnlineState = new QState(this);
	LoggedOutOfflineState = new QState(this);
	WantToLogInState = new QState(this);
	LoggingInState = new QState(this);
	LoggedInState = new QState(this);
	PasswordRequiredState = new QState(this);

	connect(LoggingOutState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LoggedOutOnlineState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LoggedOutOfflineState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(WantToLogInState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LoggingInState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LoggedInState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(PasswordRequiredState, SIGNAL(entered()), this, SLOT(printConfiguration()));

	connect(LoggingOutState, SIGNAL(entered()), this, SIGNAL(loggingOutStateEntered()));
	connect(LoggedOutOnlineState, SIGNAL(entered()), this, SIGNAL(loggedOutOnlineStateEntered()));
	connect(LoggedOutOfflineState, SIGNAL(entered()), this, SIGNAL(loggedOutOfflineStateEntered()));
	connect(WantToLogInState, SIGNAL(entered()), this, SIGNAL(wantToLogInStateEntered()));
	connect(LoggingInState, SIGNAL(entered()), this, SIGNAL(loggingInStateEntered()));
	connect(LoggedInState, SIGNAL(entered()), this, SIGNAL(loggedInStateEntered()));
	connect(PasswordRequiredState, SIGNAL(entered()), this, SIGNAL(passwordRequiredStateEntered()));

	LoggingOutState->addTransition(NetworkManager::instance(), SIGNAL(offline()), LoggedOutOfflineState);
	LoggingOutState->addTransition(CurrentProtocol, SIGNAL(stateMachineLoggedOut()), LoggedOutOnlineState);

	LoggedOutOnlineState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatus()), LoggingInState);
	LoggedOutOnlineState->addTransition(NetworkManager::instance(), SIGNAL(offline()), LoggedOutOfflineState);

	LoggedOutOfflineState->addTransition(CurrentProtocol, SIGNAL(stateMachineChangeStatus()), WantToLogInState);
	LoggedOutOfflineState->addTransition(NetworkManager::instance(), SIGNAL(online()), LoggedOutOnlineState);

	WantToLogInState->addTransition(NetworkManager::instance(), SIGNAL(online()), LoggingInState);
	WantToLogInState->addTransition(CurrentProtocol, SIGNAL(stateMachineLogout()), LoggedOutOfflineState);

	LoggingInState->addTransition(NetworkManager::instance(), SIGNAL(offline()), WantToLogInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineLoggedIn()), LoggedInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineLogout()), LoggedOutOnlineState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordRequired()), PasswordRequiredState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionError()), LoggingInState);
	LoggingInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionClosed()), LoggedOutOnlineState);

	LoggedInState->addTransition(NetworkManager::instance(), SIGNAL(offline()), WantToLogInState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineLogout()), LoggingOutState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionError()), LoggingInState);
	LoggedInState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionClosed()), LoggedOutOnlineState);

	PasswordRequiredState->addTransition(NetworkManager::instance(), SIGNAL(offline()), WantToLogInState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachineLogout()), LoggedOutOnlineState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordAvailable()), LoggingInState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachinePasswordNotAvailable()), LoggedOutOnlineState);
	PasswordRequiredState->addTransition(CurrentProtocol, SIGNAL(stateMachineConnectionClosed()), LoggedOutOnlineState);

	if (NetworkManager::instance()->isOnline())
		setInitialState(LoggedOutOnlineState);
	else
		setInitialState(LoggedOutOfflineState);

	start();
}

ProtocolStateMachine::~ProtocolStateMachine()
{
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Helper method printing current machine configuration to debug output.
 */
void ProtocolStateMachine::printConfiguration()
{
	QStringList states;
	
	if (configuration().contains(LoggingOutState))
		states.append("logging-out");
	if (configuration().contains(LoggedOutOnlineState))
		states.append("logged-out-online");
	if (configuration().contains(LoggedOutOfflineState))
		states.append("logged-out-offline");
	if (configuration().contains(WantToLogInState))
		states.append("want-to-log-in");
	if (configuration().contains(PasswordRequiredState))
		states.append("password-required");
	if (configuration().contains(LoggingInState))
		states.append("logging-in");
	if (configuration().contains(LoggedInState))
		states.append("logged-in");

	kdebugm(KDEBUG_INFO, "State machine: [%s]\n", qPrintable(states.join(", ")));
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns true is machine is currently in logged in state.
 * @return true is machine is currently in logged in state
 *
 * Returns true is machine is currently in logged in state.
 */
bool ProtocolStateMachine::isLoggedIn()
{
	return configuration().contains(LoggedInState);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns true is machine is currently in logging in state.
 * @return true is machine is currently in logging in state
 *
 * Returns true is machine is currently in logging in state.
 */
bool ProtocolStateMachine::isLoggingIn()
{
	return configuration().contains(LoggingInState);
}