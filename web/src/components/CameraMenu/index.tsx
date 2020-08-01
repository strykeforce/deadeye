import { VideoCameraOutlined, SettingOutlined } from "@ant-design/icons";
import { Menu } from "antd";
import React from "react";
import "./camera-menu.less";
import logo from "./deadeye.png";

type Props = {
  ids: string[];
  selectedId: string | undefined;
  onClick: (id: string) => void;
};

const CameraMenu = (props: Props) => {
  const { ids, selectedId, onClick } = props;

  const menuItems = ids.map((id) => (
    <Menu.Item
      key={id}
      icon={<VideoCameraOutlined />}
      onClick={() => onClick(id)}
    >
      {id}
    </Menu.Item>
  ));

  return (
    <>
      <div className="camera-menu__logo">
        <img src={logo} alt="Logo" />
      </div>
      <Menu
        className="camera-menu"
        theme="dark"
        mode="inline"
        selectedKeys={selectedId ? [selectedId] : [ids[0]]}
      >
        {menuItems}
        <Menu.Item key={ids.length + 1} icon={<SettingOutlined />}></Menu.Item>
      </Menu>
    </>
  );
};

export default CameraMenu;
