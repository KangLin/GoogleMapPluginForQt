Google map plugin for qt
-------------------------

### Building:

Get Source code into qt source code tree
    
	cd ${Qt5_ROOT}/qtlocation/src/plugins/geoservices
	git clone git@github.com:KangLin/GoogleMapPluginForQt.git

Modifiy geoservices.pro, add GoogleMapPluginForQt into SUBDIRS.


	vim geoservices.pro 
	SUBDIRS += GoogleMapPluginForQt


[Building Qt 5 from Git](http://wiki.qt.io/Building-Qt-5-from-Git) 


### [Usage](doc/google.qdoc)  




Google 地图 qt 插件
------------------

### 编译:

加入源到qt源码目录树中


    cd ${Qt5_ROOT}/qtlocation/src/plugins/geoservices
    git clone git@github.com:KangLin/GoogleMapPluginForQt.git


修改  geoservices.pro , 加入  GoogleMapPluginForQt 到 SUBDIRS.


    vim geoservices.pro 
    SUBDIRS += GoogleMapPluginForQt


[编译 qt](http://wiki.qt.io/Building-Qt-5-from-Git-SimplifiedChinese)

### [使用](doc/zh_CN/plugins/google.qdoc)
