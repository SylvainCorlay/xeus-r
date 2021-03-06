/***************************************************************************
* Copyright (c) 2022,                                          
*                                                                          
* Distributed under the terms of the GNU General Public License v3.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#include <iostream>
#include <memory>


#include <emscripten/bind.h>

#include "xeus-r/xinterpreter.hpp"
#include "xeus/xembind.hpp"


EMSCRIPTEN_BINDINGS(my_module) {
    xeus::export_core();
    using interpreter_type = xr::interpreter;
    xeus::export_kernel<interpreter_type>("xkernel");
}

