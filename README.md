# 2023 Operating System Project

## 1. Topic
- AWS환경에서의 Mini Operating System 구현 

<br>

## 2. Purpose
- 이번 프로젝트의 목적은 리눅스 시스템의 이해와 응용에 있다. 본 프로젝트는 C 언어를 통해서 리눅스 시스템을 구현함에 있다. 리눅스의 기본 동작 구조와 체계를 이해하고 동일하게 작동 가능한 시스템을 구현한다.
 또한 AWS환경을 사용하면서 클라우드 기본 동작 구조와 체계를 이해하고 동일하게 작동 가능한 시스템을 구현한다.

 <br>

## 3. Content
본 프로젝트에서 구현하는 내용은 다음과 같다.
- AWS EC2(t2.micro)에 리눅스 시스템과 동일한 파일 탐색기 구조를 구현한다. 
(자료구조 알고리즘 사용) (알고리즘 사용 없이 바로 OS모듈을 사용하는 것이 아니라 직접 구현하는 것)
- 각 팀의 모든 인원은 본인의AWS계정이 있어야 하며 EC2를 하나씩 생성해야 함
- 팀장의 EC2에는 파일 탐색기 구조 코드가 있어야 하며 팀원은 본인의 로컬환경에서 SSH프로그램을 통해 본인의EC2접속 후 EC2에 저장한 key를 사용하여 팀장의 EC2에 접속 가능해야 한다. 실제 작동이 되는 지 확인을 위해 밑에 제시된USER ID와 PASSWORD로 Pem key 없이 접속 가능하도록 설정해야 한다. (그 외 IP에서는 팀장의 EC2에 접속 불가능하게 할 것)
- 서울 리전 내 구성 (VPC)
- SSH로 접속할 수 있는 환경
- 필수 구현 명령어 : ls, cd, mkdir, cat, chmod, chown, grep
- 링크 트리 구조로 구현 (하나의 메인문에 모든 코드 담는게 아니라 헤더파일 별 분리)
- 명령어 별 수업 시간에 설명한 옵션들 구현 (ex) ls -al)
- 파일 입출력을 통해서 폴더 및 파일 생성된 현황을 저장하고 읽어야 함
- cat 명령어를 통해서 파일 생성 및 읽기 구현
- mkdir 명령어를 통해서 다수의 폴더를 동시에 생성할 수 있어야 함
- mkdir 명령어에서 다수의 폴더 생성 시 멀티스레딩을 이용하여 동시에 생성할 것
- 이 외의 명령어에서도 동시 작업 발생 시 멀티스레딩 적용
 (어느 작업에서 동시 작업이 발생하는지 잘 생각해 볼 것)
- USER ID : OSManager, PASSWORD : Dongguk**OS

<br>

## 4. Process
&rarr; C언어 main.c 파일에 필요한 모든 메소드를 작성하지 않는다. <br>
&rarr; 필요한 메소드들과 utility 기능은 헤더파일로 나누어 설계 및 구현한다. <br>
&rarr; Linux환경에서 모든 파일들을 컴파일 하여 실행 파일을 생성한 후 결과를 확인한다. <br> <br> <br>

### **구조체 관리 방법**
&rarr; 구조체를 정의하고 선언하는 헤더 파일을 만들고 <br>
&rarr; 구조체를 사용하는 다른 파일(메소드 생성 파일)에서는 해당 헤더 파일을 포함시킨다 <br>
&rarr; 구조체를 사용하는 다른 파일에서는 해당 구조체를 초기화하거나 수정할 때, 구조체에 대한 포인터를 사용하여 접근한다. <br>

<br> <br>

### **생성해야 하는 헤더 파일 목록**
#### 1) 구조체 선언 목록
#### 2) 각 종 Utility 메소드 묶음
#### 3) Utility 외 공통 필수 메소드
#### 4) Utility 메소드가 포함되어 있는 명령어 모음

<br>


<br>

# [팀원 : 김중원 - cd, cat]
 <Br>

# &#35; cd 
### 요구 사항
* 요구되는 헤더 파일 : 구조체 선언 + Utilty 메소드
* 구현 내부 메소드 2개
    * currentMove(DirectoryTree* treeDir, char* pathDir);
    * pathMove(DirectoryTree* treeDir, char* pathDir); <br>
* Utility 메소드
    * DirExistion( &rarr;dirExistion) <br>

&rarr;  위 메소드 2개 + utility 메소드를 이용하여 cd 동작

<br> <br>

# &#35; cat 
### 요구 사항
* 요구되는 헤더 파일 : 구조체 선언 + Utilty 메소드
* 타 명령어 헤더 파일
    * mkdir 명령어 구현 헤더 파일 <br>
        &rarr; MakeDir(MakeDir(DirectoryTree* TreeDir, char* NameDir, char type));
* 구현 내부 메소드 1개
    * conCatenate(DirectoryTree* treeDir, char* fName, int o); <br>
* Utility 메소드
    * DirExistion( &rarr;dirExistion) <br> 
* main.c에서 사용할 전역 변수
    * time_t ltime 
    * struct tm* today;

&rarr;  위 메소드 1개 + utility 메소드를 이용하여 cat 동작
