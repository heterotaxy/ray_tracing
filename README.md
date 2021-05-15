# ray_tracing
ray tracin gweekend を参照にしつつray_tracing プログラムの勉強

## 実行環境
- c++ 11

## 実行方法
``./a.out filename.dat > image.ppm``
で実行。
自分はmacなので下記のような簡単なスクリプトを書いて実行している。
エラー処理などはしていないので、利用する際は注意が必要。
`applescript
(*name*)`の部分は実行するパソコンいよって変更を加える必要がある。

```applescript
on run
	set fileName to POSIX path of (choose file of type {"dat"} default location (POSIX file "Users/(*name*)/C++/ray_tracing" as alias))
	set theResponse to display dialog "Please Enter the output file" default answer "" with icon note buttons {"Cancel", "Continue"} default button "Continue"
	
	set tooutput to text returned of theResponse
	tell application "Terminal"
		set currentTab to do script "cd ~/C++/ray_tracing"
		
		delay 0.3
		do script "./a.out " & fileName & " > " & tooutput in currentTab
		repeat
			delay 1
			if not (busy of currentTab) then
				do script "open " & tooutput in currentTab
				delay 0.3
				quit application "Terminal"
				exit repeat
			end if
		end repeat
	end tell
	
end run
```

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
size width //横のピクセル数
```
まず、図形を設定したのちにその図形の特徴（表面や光源）を記述する。
