# coordinate-to-image

## 요약
- Quick Draw 데이터셋을 json 파일로 변환 후 이미지로 변환

</br>
</br>

## Requirement
- openFrameworks 0.11.0([download](https://openframeworks.cc/download/))
- openFrameworks 시작 방법은 여기 참고: https://youuuu00.tistory.com/5

</br>
</br>

## 방법
- **프로젝트 생성 후 src 폴더에 main.cpp, ofApp.cpp, ofApp.h 복사**
- 그림 데이터셋의 양식은 다음과 같다.
    ```javascript
    { 
        "key_id":"5891796615823360",
        "word":"nose",
        "countrycode":"AE",
        "timestamp":"2017-03-01 20:41:36.70725 UTC",
        "recognized":true,
        "drawing":[[[129,128,129,129,130,130,131,132,132,133,133,133,133,...]]]
    }
    ```
- 저기서 drawing 항목에 그림 좌표가 저장되어 있다. drawing 포멧은 다음과 같다.
    ```javascript
    [ 
        [  // First stroke 
            [x0, x1, x2, x3, ...],
            [y0, y1, y2, y3, ...],
            [t0, t1, t2, t3, ...]
        ],
        [  // Second stroke
            [x0, x1, x2, x3, ...],
            [y0, y1, y2, y3, ...],
            [t0, t1, t2, t3, ...]
        ],
        ... // Additional strokes
    ]
    ```
    - 첫번째 대괄호는 그림 단위, 두번째 대괄호는 선단위, 세번째 대괄호는 좌표 단위이다.
    - x는 x좌표 모음, y는 y좌표 모음으로 각 1대1 매칭하면 된다.
    - t는 msec 단위로 시작점을 알려준다고 한다.

</br>

- openFramworks에서 json 파일을 열어 그림 좌표에 접근한다.
    - json 예제 소스 코드
    ```cpp
    ofJson js;
    ofFile file("drawing.json");
	if(file.exists())
	{
	    file >> js;
	    for(auto & stroke: js)
	    {
	        if(!stroke.empty())
	        {
                // 다음 점으로 이동
		        path.moveTo(stroke[0]["x"], stroke[0]["y"]);
		        for(auto & p: stroke)
		        {
                    // 이전 점에서 해당 점까지 직선을 그림
		        	path.lineTo(p["x"], p["y"]);
		        }
		    }
	    }
    }
    ```
    - <code>ofJson</code>에 파일을 넣으면 반복문으로 json 괄호 안에 접근이 가능하다.
    - <code>moveTo</code>로 다음 점으로 이동하고 <code>lineTo</code>는 다음 점까지 직선을 그린다.
        - 그림을 그릴 때 선의 시작점은 <code>moveTo</code>로 하고 나머지(해당 선의 끝점까지)는 <code>lineTo</code>를 적용한다.
        - 그리고 다음 선의 시작점은 <code>moveTo</code>로 하며 반복한다.

</br>

- 생성된 이미지 예시

    ![quickdraw_apple](https://user-images.githubusercontent.com/42198637/88624561-2c248a80-d0e2-11ea-8a5f-8ab01d6929a6.PNG)