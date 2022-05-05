# UrbanBoundary_Detector
一款基于栅格数据的城市边界识别软件，用C++实现。

# 发表文章
Zhao, C.; Li, Y.; Weng, M. A Fractal Approach to Urban Boundary Delineation Based on Raster Land Use Maps: A Case of Shanghai, China. Land 2021, 10, 941. https://doi.org/10.3390/land10090941

# 输入
包含城市（urban）分类的栅格图，像素值为整数。栅格图不应该太大，像素分辨率可以是30m-1000m，控制tif的像素总数在10^9以内，否则程序内存过大。

# 输出
城市边界矢量（shp）和栅格（tif）数据。

# 原理
1. 利用邻域扩展法识别城市实体
2. 识别城市实体边界，在边界处用不同半径的滑动模板，实现缓冲区分析，统计半径r和融合实体个数N
3. 对采样结果做三次Hermit插值，得到相等间隔的采样点ln(N)和ln(r)
4. 用1-13次多项式拟合采样点，计算每条多项式曲线的最大曲率和贝叶斯信息准则（BIC值）
5. 选择最大BIC对应的最大曲度的r作为临界半径，再做依次缓冲，得到urban cluster
6. 做urban cluster的等级规模分布图（rank-size distribution）
7. 利用层次聚类分析对等级规模分布做二分的聚类（urban and rural area）
8. 提取urban的聚类，提取边界即为城市边界

# 参考文献
[1] Cécile Tannier, Thomas I , Vuidel G , et al. A fractal approach to identifying urban boundaries[J]. Geographical Analysis, 2011, 43(2):211-227.  
[2] Tannier C , Thomas I . Defining and characterizing urban boundaries: A fractal analysis of theoretical cities and Belgian cities[J]. Computers, Environment and Urban Systems, 2013, 41(sep.):234-248.

# 改进
1. 对采样点做了三次Hermit插值，以保证对数点间隔相等
2. 采用分层聚类分析（HAC）对urban cluster做二分类，包含最大cluster的类为城市范围（没有用c++实现，有单独的py文件可以做）
3. 可输出城市范围的tif和shp文件

# 原著中矢量版城市边界识别软件
[Morpholim](https://sourcesup.renater.fr/www/morpholim/)

# 开源协议
MIT License.   
引用请注明来源[https://github.com/ly15927086342/UrbanBoundary_Detector](https://github.com/ly15927086342/UrbanBoundary_Detector)  
Author: Yu Li

