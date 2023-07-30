import json
import os, datetime, sys
import wave
import argparse
import socket

# Python3
from urllib import parse
from http.server import HTTPServer
from http.server import BaseHTTPRequestHandler

PORT = 8888

class Handler(BaseHTTPRequestHandler):
    def _set_headers(self, length):
        self.send_response(200)
        if length > 0:
            self.send_header('Content-length', str(length))
        self.end_headers()

    def _get_chunk_size(self):
        print("get chunk size")
        data = self.rfile.read(2)
        # print(data)
        
        while data[-2:] != b"\r\n":
            data += self.rfile.read(1)
        # print(data)
        return int(data[:-2], 10)

    def _get_chunk_data(self, chunk_size):
        data = self.rfile.read(chunk_size)
        print("read chunk data in function get_chunk_data")
        # print(data)
        self.rfile.read(2)
        
        return data

    def _write_wav(self, data, rates, bits, ch):
        t = datetime.datetime.utcnow()
        time = t.strftime('%Y%m%dT%H%M%SZ')
        filename = str.format('{}_{}_{}_{}.wav', time, rates, bits, ch)

        wavfile = wave.open(filename, 'wb')
        wavfile.setparams((ch, int(bits/8), rates, 0, 'NONE', 'NONE'))
        wavfile.writeframesraw(bytearray(data))
        wavfile.close()
        return filename

    # def do_POST(self):
        
    #     urlparts = parse.urlparse(self.path)
        
    #     request_file_path = urlparts.path.strip('/')
    #     total_bytes = 0
    #     sample_rates = 0
    #     bits = 0
    #     channel = 0
    #     print("Do Post......")
    #     if (request_file_path == 'upload'
    #         and self.headers.get('Transfer-Encoding', '').lower() == 'chunked'):
    #         data = []
    #         sample_rates = self.headers.get('x-audio-sample-rates', '').lower()
    #         bits = self.headers.get('x-audio-bits', '').lower()
    #         channel = self.headers.get('x-audio-channel', '').lower()
    #         sample_rates = self.headers.get('x-audio-sample-rates', '').lower()

    #         print("Audio information, sample rates: {}, bits: {}, channel(s): {}".format(sample_rates, bits, channel))
    #         # https://stackoverflow.com/questions/24500752/how-can-i-read-exactly-one-response-chunk-with-pythons-http-client
    #         while True:
    #             chunk_size = self._get_chunk_size()
    #             total_bytes += chunk_size
    #             print("Total bytes received: {}".format(total_bytes))
    #             sys.stdout.write("\033[F")
    #             if (chunk_size == 0):
    #                 break
    #             else:
    #                 chunk_data = self._get_chunk_data(chunk_size)
    #                 data += chunk_data

    #         filename = self._write_wav(data, int(sample_rates), int(bits), int(channel))
    #         self.send_response(200)
    #         self.send_header("Content-type", "text/html;charset=utf-8")
    #         self.send_header("Content-Length", str(total_bytes))
    #         self.end_headers()
    #         body = 'File {} was written, size {}'.format(filename, total_bytes)
    #         self.wfile.write(body.encode('utf-8'))


    def do_POST(self):
        urlparts = parse.urlparse(self.path)
        
        request_file_path = urlparts.path.strip('/')
        total_bytes = 0
        sample_rates = 0
        bits = 0
        channel = 0
        print("Do Post......")
        print(self.headers.get('Transfer-Encoding', '').lower())

        if (request_file_path == 'upload'
            and self.headers.get('Transfer-Encoding', '').lower() == 'chunked'):
            data = []
            sample_rates = self.headers.get('x-audio-sample-rates', '').lower()
            bits = self.headers.get('x-audio-bits', '').lower()
            channel = self.headers.get('x-audio-channel', '').lower()
            # sample_rates = self.headers.get('x-audio-sample-rates', '').lower()

            print("Audio information, sample rates: {}, bits: {}, channel(s): {}".format(sample_rates, bits, channel))
            while True:
                chunk_size = self._get_chunk_size()
                total_bytes += chunk_size
                print("Total bytes received: {}\n".format(total_bytes))
                
                if (chunk_size == 0):
                    print("chunk data is 0, end of the chunk")
                    break
                else:
                    chunk_data = self._get_chunk_data(chunk_size)
                    data += chunk_data
                    print("print chunk data")

            filename = self._write_wav(data, int(sample_rates), int(bits), int(channel))
            body = 'File {} was written, size {}'.format("filename", total_bytes)
            self.send_response(200)
            self.send_header("Content-type", "text/plain;charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            # body = 'File {} was written, size {}'.format("filename", total_bytes)
            self.wfile.write(body.encode('utf-8'))



    # POST echoes the message adding a JSON field
    # def do_POST(self):
    #     content_len = int(self.headers.getheader('content-length'))
    #     post_body = self.rfile.read(content_len)
    #     data = json.loads(post_body)
    #     print(data)

    #     parsed_path = parse.urlparse(self.path)
    #     self.send_response(200)
    #     self.end_headers()
    #     self.wfile.write(json.dumps({
    #         'method': self.command,
    #         'path': self.path,
    #         'real_path': parsed_path.query,
    #         'query': parsed_path.query,
    #         'request_version': self.request_version,
    #         'protocol_version': self.protocol_version,
    #         'body': data
    #     }).encode())
    #     return

    def do_GET(self):
        print("Do GET")
        output = "hello, world!"
        self.send_response(200)
        self.send_header('Content-type', "text/plain;charset=utf-8")
        self.send_header("Content-Length", str(len(output)))
        self.end_headers()
        self.wfile.write(output.encode('utf-8'))


def get_host_ip():
    # https://www.cnblogs.com/z-x-y/p/9529930.html
    try:
        s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        s.connect(('8.8.8.8',80))
        ip=s.getsockname()[0]
    finally:
        s.close()
    return ip

parser = argparse.ArgumentParser(description='HTTP Server save pipeline_raw_http example voice data to wav file')
parser.add_argument('--ip', '-i', nargs='?', type = str)
parser.add_argument('--port', '-p', nargs='?', type = int)
args = parser.parse_args()
if not args.ip:
    args.ip = get_host_ip()
if not args.port:
    args.port = PORT

if __name__ == '__main__':
    ip = get_host_ip()
    port=PORT
    httpd = HTTPServer((ip, port), Handler)
    

    print("Serving HTTP on {} port {}".format(ip, port));
    httpd.serve_forever()
