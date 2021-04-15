# v3.0开发笔记

# 修改QAction

## 获取action

通过`ui.[action_name]`获取，如：`ui.action_open_image`

## 获取&修改文字

```c++
this->ui.action_open_image->text();
this->ui.action_open_image->setText(QString::fromLocal8Bit("哈哈哈"));
```

## 设置是否可选

```c++
this->ui.action_open_image->setEnabled(false);
```

## 遍历一个QMenu下的所有QAction

```c++
foreach(QAction * action, this->ui.menu_2->actions())
{
    if (action->isSeparator()) continue;

    if (action == this->ui.action_draw_point) continue;

    action->setEnabled(true);
}
```
# 滚动事件

## 判断当前按钮

```c++
void ImageViewer::wheelEvent(QWheelEvent* evt)
{
    // 如果当前按下的按钮时鼠标左键
    if (evt->buttons() & Qt::LeftButton)
    {
        // TODO...
    }
}
```

