import lldb

def qformat_summary(valobj, internal_dict):
    try:
        # Call the member function as<float>()
        # Syntax for calling template member functions in LLDB is tricky:
        # Use the exact spelling from C++: "as<float>"
        float_val = valobj.CallMethod("as<float>")
        if float_val.IsValid():
            return f"{float_val.GetValue()} (qformat)"
        else:
            return "<invalid result>"
    except Exception as e:
        return f"<error: {e}>"

def __lldb_init_module(debugger, internal_dict):
    # Adjust the regex to match your qformat template name exactly
    debugger.HandleCommand(
        'type summary add -x "^qformat<.*>$" '
        '--python-function qformat_printers.qformat_summary'
    )
