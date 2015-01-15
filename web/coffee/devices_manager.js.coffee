class DevicesManager
  constructor: ->
    @devices = []
    DevicesModel = new Model '/api/devices'
    DevicesModel.index().done (d) =>
      @devices = d
      window.devices = d

    source = new EventSource('/api/events')
    source.onmessage = (ev) =>
      data = JSON.parse ev.data

      # Port Set
      if data.type == "port_set"
        d = @device(data.device_id)
        for p in d.ports when p.id == data.port_id
          p.value = data.value
          app.refresh()


  device: (id) ->
    return d for d in @devices when d.id == id
    return false


app.devicesManager = new DevicesManager()