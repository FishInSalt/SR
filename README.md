## 问题记录
---
2021-12-11  待解决
问题描述：
1、调用模型时，有从cv::Mat到Tensor以及从Tensor到cv::Mat的转换过程，转换过程中需要注意height和width的位置是否正确。

2、目前代码中似乎存在 转换过程height和width 弄反了，导致非等宽高的图片输出错误。

3、但是由于图片的预处理和后处理中把图片先转换成了等宽高的图片，所以掩盖了这个代码中的错误。为后面埋下了bug。

