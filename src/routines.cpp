#define R_NO_REMAP

#include "R.h"
#include "Rinternals.h"
#include "R_ext/Rdynload.h"

#include "xeus-r/xinterpreter.hpp"
#include "nlohmann/json.hpp"

namespace xeus_r {
namespace routines {

SEXP kernel_info_request() {
    auto info = xeus_r::get_interpreter()->kernel_info_request();
    SEXP out = PROTECT(Rf_mkString(info.dump(4).c_str()));
    Rf_classgets(out, Rf_mkString("json"));
    UNPROTECT(1);
    return out;
}

SEXP publish_stream(SEXP name_, SEXP text_) {
    auto name = CHAR(STRING_ELT(name_, 0));
    auto text = CHAR(STRING_ELT(text_, 0));
    xeus_r::get_interpreter()->publish_stream(name, text);

    return R_NilValue;
}

SEXP publish_execution_error(SEXP ename_, SEXP evalue_, SEXP trace_back_) {
    auto ename = CHAR(STRING_ELT(ename_, 0));
    auto evalue = CHAR(STRING_ELT(evalue_, 0));

    auto n = XLENGTH(trace_back_);
    std::vector<std::string> trace_back(n);
    for (decltype(n) i = 0; i < n; i++) {
        trace_back[i] = CHAR(STRING_ELT(trace_back_, i));
    }
    
    xeus_r::get_interpreter()->publish_execution_error(ename, evalue, std::move(trace_back));

    return R_NilValue;
}

SEXP publish_execution_result(SEXP execution_count_, SEXP data_, SEXP metadata_) {
    int execution_count = INTEGER_ELT(execution_count_, 0);
    auto data = nl::json::parse(CHAR(STRING_ELT(data_, 0)));
    auto metadata = nl::json::parse(CHAR(STRING_ELT(metadata_, 0)));
    
    xeus_r::get_interpreter()->publish_execution_result(
        execution_count, std::move(data), std::move(metadata)
    );

    return R_NilValue;
}

SEXP display_data(SEXP js_data, SEXP js_metadata){
    auto data = nl::json::parse(CHAR(STRING_ELT(js_data, 0)));
    auto metadata = nl::json::parse(CHAR(STRING_ELT(js_metadata, 0)));
    
    xeus_r::get_interpreter()->display_data(
        std::move(data), std::move(metadata), /* transient = */ nl::json::object()
    );

    return R_NilValue;
}

SEXP update_display_data(SEXP js_data, SEXP js_metadata){
    auto data = nl::json::parse(CHAR(STRING_ELT(js_data, 0)));
    auto metadata = nl::json::parse(CHAR(STRING_ELT(js_metadata, 0)));
    
    xeus_r::get_interpreter()->update_display_data(
        std::move(data), std::move(metadata), /* transient = */ nl::json::object()
    );

    return R_NilValue;
}

SEXP clear_output(SEXP wait_) {
    bool wait = LOGICAL_ELT(wait_, 0) == TRUE;
    xeus_r::get_interpreter()->clear_output(wait);
    return R_NilValue;
}

SEXP is_complete_request(SEXP code_) {
    std::string code = CHAR(STRING_ELT(code_, 0));
    auto is_complete = xeus_r::get_interpreter()->is_complete_request(code);

    SEXP out = PROTECT(Rf_mkString(is_complete.dump(4).c_str()));
    Rf_classgets(out, Rf_mkString("json"));
    UNPROTECT(1);
    return out;
}

}

void register_r_routines() {
    DllInfo *info = R_getEmbeddingDllInfo();

    static const R_CallMethodDef callMethods[]  = {
        {"xeusr_kernel_info_request"     , (DL_FUNC) &routines::kernel_info_request     , 0},
        {"xeusr_publish_stream"          , (DL_FUNC) &routines::publish_stream          , 2},
        {"xeusr_publish_execution_error" , (DL_FUNC) &routines::publish_execution_error , 3},
        {"xeusr_publish_execution_result", (DL_FUNC) &routines::publish_execution_result, 3},
        {"xeusr_display_data"            , (DL_FUNC) &routines::display_data            , 2},
        {"xeusr_update_display_data"     , (DL_FUNC) &routines::update_display_data     , 2},
        {"xeusr_clear_output"            , (DL_FUNC) &routines::clear_output            , 1},
        {"xeusr_is_complete_request"     , (DL_FUNC) &routines::is_complete_request     , 1},

        {NULL, NULL, 0}
    };

    R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

}
