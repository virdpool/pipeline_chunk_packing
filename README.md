# pipeline_chunk_packing
Arweave chunk packing

# deps

```
cd src_c_lib/arweave
./clone.sh
cd ../randomx
./clone.sh
```

nodejs and iced-coffee-script
```
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.3/install.sh | bash
source ~/.bashrc
source ~/.nvm/nvm.sh
nvm i 16
nvm alias default 16
npm i -g iced-coffee-script
```

# build

```
npm i
./s1_linux_build.coffee
```

# run

```
./bench_req_res.sh
./bench_pipeline.sh
```
