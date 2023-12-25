# Part 2 requires solving a non linear system of equations, which can not be solved easily.
# sympy a symbolic math library is used to solve said system of equations.
#
# p + t_i * v = p_i + t_i * v_i
# p and v have 3 unknowns each and each equation adds a unknown time t_i
# hailstone yields 3 equations
# so a total of 3 equations is required to find the solution in this constrained system of equations

import sympy
import sys

x, y, z, vx, vy, vz = sympy.symbols("x y z vx vy vz", integer=True)

eq = []
rows = open(sys.argv[1]).read().splitlines()[:3]
for i, row in enumerate(rows):
    xh, yh, zh, vxh, vyh, vzh = [int(x.strip())
                                 for x in row.replace("@", ",").split(",")]
    t = sympy.symbols(f"t_{i}", real=True)

    eq.append(x + t * vx - xh - t * vxh)
    eq.append(y + t * vy - yh - t * vyh)
    eq.append(z + t * vz - zh - t * vzh)

solution = sympy.solve(eq, dict=True)[0]

print(f"Part 2: {solution[x] + solution[y] + solution[z]}")
