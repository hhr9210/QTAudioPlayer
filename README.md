![基于QTWidget的音视频播放器](example.png)

pages 负责页面整体布局和功能组织，通常会组合和调用多个 widgets 来搭建页面界面。

widgets 负责具体的界面控件和控件内部的交互逻辑。widgets 需要用到数据或功能时，会调用 core 层的接口（如播放、获取歌曲信息等）。

core 只负责底层数据和功能，不直接和界面交互。
