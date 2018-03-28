
# cpp type name: [min value, max value, is integral type, suffix in cpp code]
types = {
    'int8_t': [-128, 127, 1, ''],
    'int16_t': [-32768, 32767, 1, ''],
    'int32_t': [-2147483648, 2147483647, 1, ''],
    # 'int64_t': [-9223372036854775808, 9223372036854775807, 1, 'l'],
    'uint8_t': [0, 255, 1, ''],
    'uint16_t': [0, 65535, 1, ''],
    'uint32_t': [0, 4294967295, 1, ''],
    # 'uint64_t': [0, 18446744073709551615, 1, 'ul'],
    'float': [0, 1, 0, 'f'],
    'double': [0, 1, 0, '']
}


def scale(x, xp, yp):
    x_range = xp[1] - xp[0]
    y_range = yp[1] - yp[0]
    r = float(y_range) / float(x_range)
    return (x - xp[0]) * r + yp[0]


def to_test_line(v, t0_name, t1_name):

    t0, t1 = types[t0_name], types[t1_name]

    f0 = '{:0.15f}'
    if t0[2] == 1:
        v = int(v)
        f0 = '{:d}'

    s = scale(v, t0, t1)

    f1 = 'Approx({:0.15f})'
    if t1[2] == 1:
        s = int(s)
        f1 = '{:d}'

    result = 'REQUIRE(TypeScaleHelper<{}, {}>::scale({}) == {});'.format(t0_name, t1_name, f0, f1).format(v, s)
    return result


def main():
    values = [-10873, -190, -42, 0, 0.2, 0.5, 0.7, 27, 42, 255, 32980]
    tests = []

    for t0_name in types.keys():
        for t1_name in types.keys():
            t0, t1 = types[t0_name], types[t1_name]
            if t0[2] == 0 and t1[2] == 1:
                # skip float -> int
                continue
            tests.extend([to_test_line(v, t0_name, t1_name) for v in values if t0[0] < v < t0[1]])

    tests = '\n'.join(sorted(list(set(tests))))
    print(tests)


if __name__ == "__main__":
    main()
