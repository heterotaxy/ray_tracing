# ray_tracing
ray tracin gweekend を参照にしつつray_tracing プログラムの勉強

## 実行環境
- c++ 11

## 実行方法
``./a.out filename.dat > image.ppm``
で実行。
macであればApplescriptで実行できいるようにした。

## filename.dataの書き方
```
sphe x y z r //球
box x0 y0 z0 x1 y1 z1//箱
plane x0 x1 y0 y1 z　axis//平面axis->0:xy平面1:yz平面2:xz平面

lambert R G B　//lambertian反射
mirr R G B fuzz
trpa n tc //屈折率　透過率
check R G B R1 G1 B1 //spheの次
check R G B R1 G1 B1 n //n:chekの数(平面)
tmap imagepath
plig R G B //光源色の設定

eyep x y z
refp z y z
back R G B
vang ha va
```
まず、図形を設定したのちにその図形の特徴（表面や光源）を記述する。
