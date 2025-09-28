DLL 인젝션 및 코드 인젝션 실습용 컨테이너 환경

```bash
sudo docker build -t dll-injection-lab .
sudo docker run -it --rm -p 8080:8080 dll-injection-lab bash -c "code-server --bind-addr 0.0.0.0:8080 --auth none /workspace"
