import typing, ext

# An innocent annotated class.
class Widget(metaclass=ext.Meta):
    pass

def f() -> typing.List[Widget]:
    ...

# Somewhere unrelated: an uncollectable cycle holding a typing annotation.
box = ext.Box()
box.ref = [box, typing.Dict[str, int]]
