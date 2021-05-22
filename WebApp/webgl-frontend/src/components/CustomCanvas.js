import React, { Component } from "react";
import * as THREE from "three";
import * as objectByteConverter from "object-byte-converter";

const scene = new THREE.Scene();
const renderer = new THREE.WebGLRenderer({ alpha: false });
const camera = new THREE.PerspectiveCamera(
  25,
  window.innerWidth / window.innerHeight,
  1,
  20000
);
const raycaster = new THREE.Raycaster();
const mouse = new THREE.Vector2();
const vec = new THREE.Vector3();
const pos = new THREE.Vector3();
let objectJson = null;
let geometry = null;
let mesh = null;

const fetchData = async (scene, renderer, camera, color) => {
  fetch("http://localhost:8081/getObject")
    .then((res) => res.json())
    .then((json) => {
      console.log("JSON:", json);
      createGeometry(json.vertices, json.faces, scene, renderer, camera, color);
      objectJson = json;
    })
    .catch((error) => console.log(error));
};

const getSelectedFaces = (newPoints) => {
  const test = {
    point1: [newPoints[0].x, newPoints[0].y, newPoints[0].z].toString(),
    point2: [newPoints[1].x, newPoints[1].y, newPoints[1].z].toString(),
    object: objectJson,
  };

  fetch("http://localhost:8081/getSelectedFaces", {
    method: "POST",
    body: btoa(JSON.stringify(test)),
  })
    .then((res) => {
      console.log("POST Response", res);
      const reader = res.body.getReader();

      return new ReadableStream({
        start(controller) {
          // The following function handles each data chunk
          function push() {
            // "done" is a Boolean and value a "Uint8Array"
            reader.read().then(({ done, value }) => {
              // If there is no more data to read
              if (done) {
                controller.close();
                return;
              }
              // Get the data and send it to the browser via the controller
              controller.enqueue(value);
              // Check chunks by logging to the console
              push();
            });
          }

          push();
        },
      });
    })
    .then((stream) => {
      // Respond with our stream
      return new Response(stream, {
        headers: { "Content-Type": "text/html" },
      }).text();
    })
    .then((result) => {
      let faces = mesh.geometry.faces;

      for (var index of JSON.parse(result).indexes) {
        faces[index].color.setHex(0x03fc13);
      }

      mesh.geometry.colorsNeedUpdate = true;
      renderer.render(scene, camera);
    })
    .catch((error) => {
      console.log("Error: ", error);
    });
};

const createGeometry = (vertices, faces, scene, renderer, camera, color) => {
  geometry = new THREE.Geometry();

  vertices.forEach((vertex) => {
    const vertexVector = new THREE.Vector3(vertex[0], vertex[1], vertex[2]);
    geometry.vertices.push(vertexVector);
  });

  faces.forEach((face) => {
    const faceVector = new THREE.Face3(face[0], face[1], face[2]);
    geometry.faces.push(faceVector);
  });

  const material = new THREE.MeshPhongMaterial({
    color: color,
    specular: 0x111111,
    shininess: 200,
    vertexColors: THREE.FaceColors,
  });

  const object = new THREE.Mesh(geometry, material);

  mesh = object;

  scene.add(object);

  geometry.computeVertexNormals(true);

  renderer.render(scene, camera);

  window.addEventListener("mousedown", onMouseClick, false);
  window.addEventListener("keydown", rotateCamera, false);
};

const rotateCamera = (event) => {
  var rotSpeed = 0.02;
  var x = camera.position.x,
    y = camera.position.y,
    z = camera.position.z;

  if (event.keyCode == 37) {
    // left arrow key code
    camera.position.x = x * Math.cos(rotSpeed) + y * Math.sin(rotSpeed);
    camera.position.y = y * Math.cos(rotSpeed) - x * Math.sin(rotSpeed);
    camera.rotateY(-rotSpeed);
  } else if (event.keyCode == 38) {
    // up arrow key code
    camera.position.y = y * Math.cos(rotSpeed) + z * Math.sin(rotSpeed);
    camera.position.z = z * Math.cos(rotSpeed) - y * Math.sin(rotSpeed);
    camera.rotateX(-rotSpeed);
  } else if (event.keyCode == 39) {
    // right arrow key code
    camera.position.x = x * Math.cos(-rotSpeed) + y * Math.sin(-rotSpeed);
    camera.position.y = y * Math.cos(-rotSpeed) - x * Math.sin(-rotSpeed);
    camera.rotateY(rotSpeed);
  } else if (event.keyCode == 40) {
    // down arrow key code
    camera.position.y = y * Math.cos(-rotSpeed) + z * Math.sin(-rotSpeed);
    camera.position.z = z * Math.cos(-rotSpeed) - y * Math.sin(-rotSpeed);
    camera.rotateX(rotSpeed);
  }

  renderer.render(scene, camera);
};

const onMouseClick = (event) => {
  const points = [];
  const material = new THREE.LineBasicMaterial({ color: 0x0000ff });

  vec.set(
    (event.clientX / window.innerWidth) * 2 - 1,
    -(event.clientY / window.innerHeight) * 2 + 1,
    0.5
  );

  vec.unproject(camera);

  vec.sub(camera.position).normalize();

  var distance = -camera.position.z / vec.z;
  pos.copy(camera.position).add(vec.multiplyScalar(distance));

  points.push(new THREE.Vector3(pos.x, pos.y, pos.z));
  points.push(
    new THREE.Vector3(camera.position.x, camera.position.y, camera.position.z)
  );

  const geometry = new THREE.BufferGeometry().setFromPoints(points);
  const line = new THREE.Line(geometry, material);
  scene.add(line);

  getSelectedFaces(points);
};

class CustomCanvas extends Component {
  componentDidMount() {
    console.log("New Color", this.props.objectColor);
    renderer.setClearColor(0xc5c5c3);
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);

    camera.position.x = 0;
    camera.position.y = -100;
    camera.position.z = 25;

    camera.lookAt(0, 0, 0);

    raycaster.setFromCamera(mouse, camera);

    var ambientLight = new THREE.AmbientLight(0x404040);
    scene.add(ambientLight);

    var directionalLight = new THREE.DirectionalLight(0x404040);
    directionalLight.position.set(1, -1, 1).normalize();
    scene.add(directionalLight);

    const axesHelper = new THREE.AxesHelper(1000);
    scene.add(axesHelper);

    fetchData(scene, renderer, camera, this.props.objectColor);
  }

  render() {
    return <div id="canvas"></div>;
  }
}

export default CustomCanvas;
