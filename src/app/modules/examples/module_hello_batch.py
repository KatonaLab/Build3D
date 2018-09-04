from a3dc_module_interface import def_process_module


def module_main(ctx):
    print('Hello Batch Runner, your run_id is {}'.format(ctx.run_id()))
    if ctx.run_id() < 8:
        ctx.set_require_next_run(True)
    else:
        ctx.set_require_next_run(False)


def_process_module([], module_main)
