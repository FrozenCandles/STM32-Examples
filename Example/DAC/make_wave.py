
"""
usage:
$ python make_wave.py 
"""

import numpy as np

# 修改本变量可以更改点数，如16、32、64等
POINT_NUM = 32

n = np.linspace(0, 2 * np.pi, POINT_NUM)
sin_val = np.sin(n)

# 调整幅值至在0~1区间
sin_val += 1
# 按3.3V电压调整幅值
sin_val *= 3.3 / 2
#求取dac数值，12位dac LSB = 3.3V/2**12
sin_val = np.round(sin_val * 2 ** 12 / 3.3)
sin_val[sin_val > 4095] = 4095
sin_val = sin_val.astype(np.uint32)

print(sin_val)

with open("sin_wave.txt", 'w', encoding='utf-8') as f:
    f.write(', '.join(sin_val.astype(np.str_)))

try:
    import matplotlib.pyplot as plt
except ImportError:
    pass
else:
    plt.plot(n, sin_val, "-o")
    plt.show()
