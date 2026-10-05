#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"

using namespace std::chrono_literals;
using visualization_msgs::msg::Marker;

// Egy fazis: nev, idotartam [s] es hogy melyik lampa vilagit
struct Phase
{
  std::string name;
  double duration;
  bool red, yellow, green;
};

class TrafficLight : public rclcpp::Node
{
public:
  TrafficLight() : Node("traffic_light_node")
  {
    // A fazisok hossza parameterkent allithato
    phases_ = {
      {"RED",        this->declare_parameter("red_time", 5.0),        true,  false, false},
      {"RED_YELLOW", this->declare_parameter("red_yellow_time", 1.5), true,  true,  false},
      {"GREEN",      this->declare_parameter("green_time", 5.0),      false, false, true},
      {"YELLOW",     this->declare_parameter("yellow_time", 2.0),     false, true,  false}};
    state_pub_ = this->create_publisher<std_msgs::msg::String>("traffic_light/state", 10);
    marker_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>("traffic_light/markers", 10);
    phase_start_ = this->now();
    timer_ = this->create_wall_timer(100ms, std::bind(&TrafficLight::timer_callback, this));
    RCLCPP_INFO(this->get_logger(), "Traffic light started, phase: %s", phases_[current_].name.c_str());
  }

private:
  void timer_callback()
  {
    // Allapotgep: ha letelt a fazis ideje, lepes a kovetkezore
    if ((this->now() - phase_start_).seconds() >= phases_[current_].duration) {
      current_ = (current_ + 1) % phases_.size();
      phase_start_ = this->now();
      RCLCPP_INFO(this->get_logger(), "Phase changed to: %s", phases_[current_].name.c_str());
    }
    const Phase & p = phases_[current_];
    std_msgs::msg::String state;
    state.data = p.name;
    state_pub_->publish(state);

    visualization_msgs::msg::MarkerArray arr;
    arr.markers.push_back(make_marker(0, Marker::CYLINDER, 0.0, 0.75, {0.08, 0.08, 1.5}, {0.4, 0.4, 0.4}));
    arr.markers.push_back(make_marker(1, Marker::CUBE, 0.0, 1.95, {0.25, 0.35, 0.9}, {0.1, 0.1, 0.1}));
    // Lampak: bekapcsolva elenk, kikapcsolva halvany szin
    arr.markers.push_back(make_marker(2, Marker::SPHERE, 0.14, 2.25, {0.22, 0.22, 0.22}, p.red ? Rgb{1.0, 0.0, 0.0} : Rgb{0.25, 0.0, 0.0}));
    arr.markers.push_back(make_marker(3, Marker::SPHERE, 0.14, 1.95, {0.22, 0.22, 0.22}, p.yellow ? Rgb{1.0, 0.8, 0.0} : Rgb{0.25, 0.2, 0.0}));
    arr.markers.push_back(make_marker(4, Marker::SPHERE, 0.14, 1.65, {0.22, 0.22, 0.22}, p.green ? Rgb{0.0, 1.0, 0.0} : Rgb{0.0, 0.25, 0.0}));
    Marker text = make_marker(5, Marker::TEXT_VIEW_FACING, 0.0, 2.65, {0.0, 0.0, 0.2}, {1.0, 1.0, 1.0});
    text.text = p.name;
    arr.markers.push_back(text);
    marker_pub_->publish(arr);
  }

  struct Rgb { double r, g, b; };
  struct Xyz { double x, y, z; };

  Marker make_marker(int id, int type, double x, double z, Xyz scale, Rgb color)
  {
    Marker m;
    m.header.frame_id = "map";
    m.header.stamp = this->now();
    m.ns = "traffic_light";
    m.id = id;
    m.type = type;
    m.action = Marker::ADD;
    m.pose.position.x = x;
    m.pose.position.z = z;
    m.pose.orientation.w = 1.0;
    m.scale.x = scale.x;
    m.scale.y = scale.y;
    m.scale.z = scale.z;
    m.color.r = color.r;
    m.color.g = color.g;
    m.color.b = color.b;
    m.color.a = 1.0;
    return m;
  }

  std::vector<Phase> phases_;
  size_t current_ = 0;
  rclcpp::Time phase_start_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr state_pub_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TrafficLight>());
  rclcpp::shutdown();
  return 0;
}