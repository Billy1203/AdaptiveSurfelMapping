# This is a Python tool for transform the ROTATION information in different forms
## 1. Rotation matrix <==  Eular angle
## 2. Rotation matrix <==> Angle axis
## 3. Rotation matrix <==> Quaternion

import numpy as np
import math

# Rotation matrix ==> Angle axis
def rotmat_to_angleaxis(R):
    theta = math.acos((R[0,0] + R[1,1] + R[2,2] - 1)/2 )
    direct = np.array([[R[2,1] - R[1,2]], [R[0,2] - R[2,0]], [R[1,0] - R[0,1]]
                      ])
    direct /= 2*math.sin(theta)
    
    return theta, direct




# Rotation matrix <==  Eular angle : extrinsic Eular
def eularangle_to_rotmate(theta):
    '''extrinsic rotations, x-y-z. theta_x:(-pi,pi), theta_y:(-pi/2,pi/2), theta_z:(-pi,pi)'''
    # theta a 3 sized list with angles

    R_x = np.array([[1,         0,                  0                   ],
                    [0,         math.cos(theta[0]), -math.sin(theta[0]) ],
                    [0,         math.sin(theta[0]), math.cos(theta[0])  ]
                    ])
        
    R_y = np.array([[math.cos(theta[1]),    0,      math.sin(theta[1])  ],
                    [0,                     1,      0                   ],
                    [-math.sin(theta[1]),   0,      math.cos(theta[1])  ]
                    ])
                
    R_z = np.array([[math.cos(theta[2]),    -math.sin(theta[2]),    0],
                    [math.sin(theta[2]),    math.cos(theta[2]),     0],
                    [0,                     0,                      1]
                    ])
                    
    R = np.dot(R_z, np.dot( R_y, R_x ))  # Rz*Ry*Rx

    return R

# Rotation matrix <==  Eular angle : intrinsic Eular
def eularangle_to_rotmati(theta):
    '''intrinsic rotations, x-y'-z''. theta_x:(-pi,pi), theta_y:(-pi/2,pi/2), theta_z:(-pi,pi)'''
    # theta a 3 sized list with angles

    R_x = np.array([[1,         0,                  0                   ],
                    [0,         math.cos(theta[0]), -math.sin(theta[0]) ],
                    [0,         math.sin(theta[0]), math.cos(theta[0])  ]
                    ])

    R_y = np.array([[math.cos(theta[1]),    0,      math.sin(theta[1])  ],
                    [0,                     1,      0                   ],
                    [-math.sin(theta[1]),   0,      math.cos(theta[1])  ]
                    ])
                
    R_z = np.array([[math.cos(theta[2]),    -math.sin(theta[2]),    0],
                    [math.sin(theta[2]),    math.cos(theta[2]),     0],
                    [0,                     0,                      1]
                    ])
                    
    R = np.dot(np.dot( R_x, R_y ), R_z)  # Rx*Ry*Rz

    return R


# Angle axis <==  Eular angle : entrinsic Eular
def eularangle_to_angleaxise(theta):
    '''extrinsic rotations, x-y-z. theta_x:(-pi,pi), theta_y:(-pi/2,pi/2), theta_z:(-pi,pi)'''
    # theta a 3 sized list with angles
    
    return rotmat_to_angleaxis(eularangle_to_rotmate(theta))

# Angle axis <==  Eular angle : intrinsic Eular
def eularangle_to_angleaxisi(theta):
    '''intrinsic rotations, x-y'-z''. theta_x:(-pi,pi), theta_y:(-pi/2,pi/2), theta_z:(-pi,pi)'''
    # theta a 3 sized list with angles
    
    return rotmat_to_angleaxis(eularangle_to_rotmati(theta))


# Rotation matrix <== Quaternion
def quaternion_to_rotmat(q):
    '''@q quaternion in [x,y,z,w]'''
    x = q[0]
    y = q[1]
    z = q[2]
    w = q[3]
    
    double_xy = 2*x*y
    double_yz = 2*y*z
    double_xz = 2*x*z
    double_xw = 2*x*w
    double_yw = 2*y*w
    double_zw = 2*z*w
    double_xx = 2*x*x
    double_yy = 2*y*y
    double_zz = 2*z*z
    
    R = np.array([[1-double_yy-double_zz, double_xy-double_zw  , double_xz+double_yw  ],
                  [double_xy+double_zw  , 1-double_xx-double_zz, double_yz-double_xw  ],
                  [double_xz-double_yw  , double_yz+double_xw  , 1-double_xx-double_yy]])
    return R
    
# Rotation matrix ==> Quaternion
def rotmat_to_quaternion(R):
    trace = R[0,0] + R[1,1] + R[2,2]
    qw = 0.5 * math.sqrt(trace + 1)
    i4qw = 1.0 / (4*qw)
    qx = (R[1,2]-R[2,1]) * i4qw
    qy = (R[2,0]-R[0,2]) * i4qw
    qz = (R[0,1]-R[1,0]) * i4qw

    return np.array([qx, qy, qz, qw])



# Compute the average quaternion of a bunch of quaternions. Simplified version
## Note: this only works if all the quaternions are relatively close together.
def quaternion_mean_simple(quats):
    # check if all quaternions have the same sign
    q0 = quats[0]
    for i in range(1, len(quats)):
        if np.inner(q0, quats[i]) < 0.0:
            quats[i] *= -1 # invert the sign of that quaternion

    # get the mean of each element
    mean = np.sum(quats, axis=0)
    # normalize
    return mean / np.linalg.norm(mean)
