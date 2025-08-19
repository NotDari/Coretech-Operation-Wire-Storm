//
// Created by Dariush Tomlinson on 12/08/2025.
//

#pragma once

/**
 * Enum of potential errors that could occur withing the application.
 * Its used to have categorise errors and handle different error scenarios.
 *
 * Members:
 * DEFAULT - Used in the case a specific error isn't necessary or isn't known.
 * CONNECTION_CLOSED - Used when a socket is attempting to be accessed but has already been closed.
 * BROKEN_PIPE - Used when trying to send data to a destination which is no longer connected.
 * STOP_THREAD - Used when a thread has been requested to stop and is following that order.
 * NO_CONNECTION_ATTEMPT - Used when checking if a specific socket has no data available but hasn't thrown an error.
 */
enum ErrorCode {
    DEFAULT,
    CONNECTION_CLOSED,
    BROKEN_PIPE,
    STOP_THREAD,
    NO_CONNECTION_ATTEMPT
};
