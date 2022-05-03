import { SettingOutlined, VideoCameraOutlined } from "@ant-design/icons";
import { useNavigate } from "@gatsbyjs/reach-router";
import { Menu } from "antd";
import React, { useState } from "react";
import "./camera-menu.less";
import logo from "./deadeye.png";


const CameraMenu = (props) => {
  const { id, ids } = props;
  const [selectedId, setSelectedId] = useState(id);
  const navigate = useNavigate();

  const handleClick = (id) => {
    setSelectedId(id);
    void navigate(`/id/${id}`);
  };

  const menuItems = ids.map((id) => (
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
        <Menu.Item
          key="settings"
          icon={<SettingOutlined />}
          onClick={() => void navigate("/settings")}
        />
      </Menu>
    </>
  );
};

export default CameraMenu;
