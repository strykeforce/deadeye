import { VideoCameraOutlined, SettingOutlined } from "@ant-design/icons";
import { Menu } from "antd";
import React, { useState } from "react";
import "./camera-menu.less";
import logo from "./deadeye.png";
import { Link, useNavigate } from "@reach/router";
import { Id } from "../../common/models";

type Props = {
  ids?: string[];
};

const CameraMenu = (props: Props) => {
  const { ids } = props;
  const [selectedId, setSelectedId] = useState<Id>(ids && ids[0]);
  const navigate = useNavigate();

  const handleClick = (id: string) => {
    setSelectedId(id);
    navigate(`/id/${id}`);
  };

  const cameras = ids ? ids : [];

  const menuItems = cameras.map((id) => (
    <Menu.Item
      key={id}
      icon={<VideoCameraOutlined />}
      onClick={() => handleClick(id)}
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
        selectedKeys={selectedId ? [selectedId] : undefined}
      >
        {menuItems}
        <Menu.Item key="settings" icon={<SettingOutlined />}></Menu.Item>
      </Menu>
    </>
  );
};

export default CameraMenu;
